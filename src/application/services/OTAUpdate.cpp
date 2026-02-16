#include "application/services/OTAUpdate.h"

#include <ArduinoJson.h>
#include <Arduino.h>

#include "config/NetworkConfig.h"
#include "config/Version.h"

#ifndef BOARD_SIMULATOR
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <mbedtls/md.h>
#endif

OTAUpdate::OTAUpdate(INetwork *network, const char *baseUrl,
                     const char *secretKey)
    : _network(network), _baseUrl(baseUrl), _secretKey(secretKey) {}

void OTAUpdate::setProgressCallback(OTAProgressCallback cb) {
  _progressCb = cb;
}

OTAStatus OTAUpdate::status() const { return _status; }

const String &OTAUpdate::availableVersion() const {
  return _availableVersion;
}

bool OTAUpdate::checkForUpdate() {
  _status = OTAStatus::Checking;

  String url = String(_baseUrl) + "/api/version?board=" + BOARD_ID;
  HttpResponse resp = _network->get(url.c_str());

  if (resp.statusCode != 200) {
    Serial.printf("[OTA] Version check failed: HTTP %d\n", resp.statusCode);
    _status = OTAStatus::Error;
    return false;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, resp.body);
  if (err) {
    Serial.printf("[OTA] JSON parse error: %s\n", err.c_str());
    _status = OTAStatus::Error;
    return false;
  }

  const char *remoteVersion = doc["version"];
  if (remoteVersion == nullptr) {
    Serial.println("[OTA] Missing version field in response");
    _status = OTAStatus::Error;
    return false;
  }

  _availableVersion = String(remoteVersion);
  _expectedHmac = String(doc["hmac"].as<const char *>());
  _expectedSize = doc["size"] | 0;

  // Compare versions: parse major.minor.patch as integers
  int localMajor = 0, localMinor = 0, localPatch = 0;
  int remoteMajor = 0, remoteMinor = 0, remotePatch = 0;
  sscanf(FIRMWARE_VERSION, "%d.%d.%d", &localMajor, &localMinor, &localPatch);
  sscanf(remoteVersion, "%d.%d.%d", &remoteMajor, &remoteMinor, &remotePatch);

  bool isNewer = (remoteMajor > localMajor) ||
                 (remoteMajor == localMajor && remoteMinor > localMinor) ||
                 (remoteMajor == localMajor && remoteMinor == localMinor &&
                  remotePatch > localPatch);

  if (isNewer) {
    Serial.printf("[OTA] Update available: %s -> %s\n", FIRMWARE_VERSION,
                  remoteVersion);
    _status = OTAStatus::UpdateAvailable;
    return true;
  }

  Serial.printf("[OTA] Up to date (local=%s, remote=%s)\n", FIRMWARE_VERSION,
                remoteVersion);
  _status = OTAStatus::NoUpdate;
  return false;
}

bool OTAUpdate::performUpdate() {
#ifdef BOARD_SIMULATOR
  Serial.println("[OTA] Simulator: firmware flash not available.");
  _status = OTAStatus::Error;
  return false;
#else
  return downloadAndFlash();
#endif
}

#ifndef BOARD_SIMULATOR
bool OTAUpdate::downloadAndFlash() {
  _status = OTAStatus::Downloading;

  String url = String(_baseUrl) + "/api/firmware?board=" + BOARD_ID;

  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(30000);
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode != 200) {
    Serial.printf("[OTA] Firmware download failed: HTTP %d\n", httpCode);
    http.end();
    _status = OTAStatus::Error;
    return false;
  }

  int contentLength = http.getSize();
  if (contentLength <= 0) {
    Serial.println("[OTA] Invalid content length");
    http.end();
    _status = OTAStatus::Error;
    return false;
  }

  Serial.printf("[OTA] Downloading %d bytes...\n", contentLength);

  WiFiClient *stream = http.getStreamPtr();
  if (!Update.begin(contentLength)) {
    Serial.printf("[OTA] Not enough space: %s\n", Update.errorString());
    http.end();
    _status = OTAStatus::Error;
    return false;
  }

  // Set up incremental HMAC-SHA256
  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
  mbedtls_md_hmac_starts(&ctx, (const unsigned char *)_secretKey,
                         strlen(_secretKey));

  uint8_t buf[4096];
  size_t written = 0;
  int lastPct = -1;
  unsigned long lastDataTime = millis();
  const unsigned long STALL_TIMEOUT_MS = 15000;

  while (http.connected() && written < (size_t)contentLength) {
    size_t available = stream->available();
    if (available) {
      size_t toRead = available < sizeof(buf) ? available : sizeof(buf);
      int bytesRead = stream->readBytes(buf, toRead);
      if (bytesRead <= 0) break;

      size_t writeResult = Update.write(buf, bytesRead);
      if (writeResult != (size_t)bytesRead) {
        Serial.printf("[OTA] Flash write failed at %u bytes: %s\n",
                      (unsigned)written, Update.errorString());
        Update.abort();
        mbedtls_md_free(&ctx);
        http.end();
        _status = OTAStatus::Error;
        return false;
      }

      mbedtls_md_hmac_update(&ctx, buf, bytesRead);
      written += bytesRead;
      lastDataTime = millis();

      int pct = (written * 100) / contentLength;
      if (pct != lastPct) {
        lastPct = pct;
        Serial.printf("[OTA] %d%%\r", pct);
        if (_progressCb) {
          _progressCb(pct);
        }
      }
    } else if (millis() - lastDataTime > STALL_TIMEOUT_MS) {
      Serial.printf("\n[OTA] Download stalled at %u/%d bytes\n",
                    (unsigned)written, contentLength);
      Update.abort();
      mbedtls_md_free(&ctx);
      http.end();
      _status = OTAStatus::Error;
      return false;
    }
    delay(1);
  }

  Serial.println();

  if (written != (size_t)contentLength) {
    Serial.printf("[OTA] Incomplete download: %u/%d\n", (unsigned)written,
                  contentLength);
    Update.abort();
    mbedtls_md_free(&ctx);
    http.end();
    _status = OTAStatus::Error;
    return false;
  }

  // Verify HMAC
  _status = OTAStatus::Verifying;
  unsigned char hmacResult[32];
  mbedtls_md_hmac_finish(&ctx, hmacResult);
  mbedtls_md_free(&ctx);

  char hexStr[65];
  for (int i = 0; i < 32; i++) {
    sprintf(hexStr + i * 2, "%02x", hmacResult[i]);
  }
  hexStr[64] = '\0';

  if (_expectedHmac != hexStr) {
    Serial.println("[OTA] HMAC verification FAILED — aborting!");
    Serial.printf("[OTA] Expected: %s\n", _expectedHmac.c_str());
    Serial.printf("[OTA] Got:      %s\n", hexStr);
    Update.abort();
    http.end();
    _status = OTAStatus::Error;
    return false;
  }

  Serial.println("[OTA] HMAC verified OK");

  if (!Update.end(true)) {
    Serial.printf("[OTA] Finalize failed: %s\n", Update.errorString());
    http.end();
    _status = OTAStatus::Error;
    return false;
  }

  http.end();
  Serial.println("[OTA] Update successful!");
  _status = OTAStatus::Success;
  return true;
}
#endif
