#ifndef _OTA_UPDATE_H_
#define _OTA_UPDATE_H_

#include "device/INetwork.h"

#include <functional>

enum class OTAStatus {
  Idle,
  Checking,
  NoUpdate,
  UpdateAvailable,
  Downloading,
  Verifying,
  Success,
  Error
};

// Called during download with percentage (0-100)
using OTAProgressCallback = std::function<void(int pct)>;

class OTAUpdate {
private:
  INetwork *_network;
  const char *_baseUrl;
  const char *_secretKey;
  String _availableVersion;
  String _expectedHmac;
  int _expectedSize = 0;
  OTAStatus _status = OTAStatus::Idle;
  OTAProgressCallback _progressCb;

  bool downloadAndFlash();

public:
  OTAUpdate(INetwork *network, const char *baseUrl, const char *secretKey);

  // Check the version endpoint. Returns true if an update is available.
  bool checkForUpdate();

  // Version string of the available update (valid after checkForUpdate returns true).
  const String &availableVersion() const;

  // Current status.
  OTAStatus status() const;

  // Set a callback to receive download progress (0-100).
  void setProgressCallback(OTAProgressCallback cb);

  // Download, verify HMAC, and flash the update.
  // Blocks until complete. Returns true on success (caller should reboot).
  bool performUpdate();
};

#endif // _OTA_UPDATE_H_
