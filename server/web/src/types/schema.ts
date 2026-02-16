export interface PropDef {
  type: "string" | "int" | "color" | "enum" | "icon";
  default: unknown;
  label: string;
  location?: "top";
  min?: number;
  max?: number;
  options?: string[];
}

export interface ComponentSchema {
  container: boolean;
  props: Record<string, PropDef>;
  fields: Record<string, PropDef>;
}

export type SchemaMap = Record<string, ComponentSchema>;

export const LVGL_SYMBOLS: { code: string; label: string }[] = [
  { code: "\uF001", label: "music" },
  { code: "\uF008", label: "film" },
  { code: "\uF00B", label: "th-list" },
  { code: "\uF00C", label: "check" },
  { code: "\uF00D", label: "close" },
  { code: "\uF011", label: "power" },
  { code: "\uF013", label: "gear" },
  { code: "\uF015", label: "home" },
  { code: "\uF019", label: "download" },
  { code: "\uF01C", label: "inbox" },
  { code: "\uF021", label: "refresh" },
  { code: "\uF026", label: "volume-off" },
  { code: "\uF027", label: "volume-down" },
  { code: "\uF028", label: "volume-up" },
  { code: "\uF03E", label: "image" },
  { code: "\uF043", label: "tint" },
  { code: "\uF048", label: "step-backward" },
  { code: "\uF04B", label: "play" },
  { code: "\uF04C", label: "pause" },
  { code: "\uF04D", label: "stop" },
  { code: "\uF051", label: "step-forward" },
  { code: "\uF052", label: "eject" },
  { code: "\uF053", label: "chevron-left" },
  { code: "\uF054", label: "chevron-right" },
  { code: "\uF067", label: "plus" },
  { code: "\uF068", label: "minus" },
  { code: "\uF06E", label: "eye" },
  { code: "\uF070", label: "eye-slash" },
  { code: "\uF071", label: "warning" },
  { code: "\uF074", label: "shuffle" },
  { code: "\uF077", label: "chevron-up" },
  { code: "\uF078", label: "chevron-down" },
  { code: "\uF079", label: "loop" },
  { code: "\uF07B", label: "folder" },
  { code: "\uF093", label: "upload" },
  { code: "\uF095", label: "phone" },
  { code: "\uF0C4", label: "cut" },
  { code: "\uF0C5", label: "copy" },
  { code: "\uF0C7", label: "save" },
  { code: "\uF0C9", label: "menu" },
  { code: "\uF0E0", label: "envelope" },
  { code: "\uF0E7", label: "bolt" },
  { code: "\uF0EA", label: "clipboard" },
  { code: "\uF0F3", label: "bell" },
  { code: "\uF11C", label: "keyboard" },
  { code: "\uF124", label: "location" },
  { code: "\uF15B", label: "file" },
  { code: "\uF1EB", label: "wifi" },
  { code: "\uF240", label: "battery-full" },
  { code: "\uF241", label: "battery-3/4" },
  { code: "\uF242", label: "battery-1/2" },
  { code: "\uF243", label: "battery-1/4" },
  { code: "\uF244", label: "battery-empty" },
  { code: "\uF287", label: "usb" },
  { code: "\uF293", label: "bluetooth" },
  { code: "\uF2DB", label: "microchip" },
  { code: "\uF2ED", label: "trash" },
  { code: "\uF55A", label: "backspace" },
];

export const THEME_COLORS: { label: string; value: string }[] = [
  { label: "White", value: "0xFFFFFF" },
  { label: "Zinc 50", value: "0xFAFAFA" },
  { label: "Zinc 100", value: "0xF4F4F5" },
  { label: "Zinc 200", value: "0xE4E4E7" },
  { label: "Zinc 300", value: "0xD4D4D8" },
  { label: "Zinc 400", value: "0xA1A1AA" },
  { label: "Zinc 500", value: "0x71717A" },
  { label: "Zinc 700", value: "0x3F3F46" },
  { label: "Zinc 800", value: "0x27272A" },
  { label: "Zinc 900", value: "0x18181B" },
  { label: "Zinc 950", value: "0x09090B" },
  { label: "Black", value: "0x000000" },
  { label: "Green", value: "0x22C55E" },
  { label: "Red", value: "0xEF4444" },
  { label: "Blue", value: "0x3B82F6" },
  { label: "Amber", value: "0xF59E0B" },
];
