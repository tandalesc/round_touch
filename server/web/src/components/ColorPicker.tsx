import { THEME_COLORS } from "../types/schema";

interface Props {
  value: string; // "0xRRGGBB" or ""
  onChange: (value: string) => void;
}

function hexToNative(hex: string): string {
  if (hex.startsWith("0x") || hex.startsWith("0X")) {
    return "#" + hex.slice(2).padStart(6, "0");
  }
  return "#000000";
}

function nativeToHex(native: string): string {
  return "0x" + native.slice(1).toUpperCase();
}

export default function ColorPicker({ value, onChange }: Props) {
  return (
    <div>
      <div className="color-input-row">
        <input
          type="text"
          value={value}
          placeholder="0xRRGGBB"
          onChange={(e) => onChange(e.target.value)}
          style={{ flex: 1 }}
        />
        <input
          type="color"
          value={value ? hexToNative(value) : "#000000"}
          onChange={(e) => onChange(nativeToHex(e.target.value))}
        />
      </div>
      <div className="color-presets">
        {THEME_COLORS.map((c) => (
          <div
            key={c.value}
            className={`color-swatch${value === c.value ? " active" : ""}`}
            style={{ backgroundColor: hexToNative(c.value) }}
            title={c.label}
            onClick={() => onChange(c.value)}
          />
        ))}
      </div>
    </div>
  );
}
