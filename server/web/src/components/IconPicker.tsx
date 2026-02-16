import { useState, useRef, useEffect } from "react";
import { LVGL_SYMBOLS } from "../types/schema";

interface Props {
  value: string;
  onChange: (value: unknown) => void;
}

export default function IconPicker({ value, onChange }: Props) {
  const [open, setOpen] = useState(false);
  const ref = useRef<HTMLDivElement>(null);

  // Close on outside click
  useEffect(() => {
    if (!open) return;
    const handler = (e: MouseEvent) => {
      if (ref.current && !ref.current.contains(e.target as Node)) {
        setOpen(false);
      }
    };
    document.addEventListener("mousedown", handler);
    return () => document.removeEventListener("mousedown", handler);
  }, [open]);

  const currentSymbol = LVGL_SYMBOLS.find((s) => s.code === value);

  return (
    <div ref={ref} style={{ position: "relative" }}>
      <div className="color-input-row">
        <input
          type="text"
          value={value}
          onChange={(e) => onChange(e.target.value)}
          placeholder="Icon character"
          style={{ flex: 1 }}
        />
        <button className="small" onClick={() => setOpen(!open)}>
          {currentSymbol ? (
            <span className="tab-icon">{currentSymbol.code}</span>
          ) : (
            "Pick"
          )}
        </button>
      </div>

      {open && (
        <div className="icon-picker-grid">
          {LVGL_SYMBOLS.map((sym) => (
            <div
              key={sym.code}
              className={`icon-picker-cell${value === sym.code ? " active" : ""}`}
              onClick={() => {
                onChange(sym.code);
                setOpen(false);
              }}
              title={sym.label}
            >
              <span className="tab-icon">{sym.code}</span>
              <span className="icon-picker-label">{sym.label}</span>
            </div>
          ))}
        </div>
      )}
    </div>
  );
}
