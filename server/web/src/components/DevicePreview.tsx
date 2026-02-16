import type { ComponentNode } from "../types/manifest";

function hexToCSS(hex: unknown): string | undefined {
  if (typeof hex !== "string" && typeof hex !== "number") return undefined;
  const s = String(hex);
  if (s.startsWith("0x") || s.startsWith("0X")) {
    return "#" + s.slice(2).padStart(6, "0");
  }
  return undefined;
}

function PreviewNode({
  node,
  path,
  selectedPath,
  onSelect,
}: {
  node: ComponentNode;
  path: number[];
  selectedPath: number[] | null;
  onSelect: (path: number[]) => void;
}) {
  const isSelected =
    selectedPath !== null &&
    path.length === selectedPath.length &&
    path.every((v, i) => v === selectedPath[i]);

  const outline = isSelected ? "2px solid #3b82f6" : "2px solid transparent";

  const renderChildren = () =>
    node.children?.map((child, i) => (
      <PreviewNode
        key={i}
        node={child}
        path={[...path, i]}
        selectedPath={selectedPath}
        onSelect={onSelect}
      />
    ));

  switch (node.type) {
    case "FillScreen": {
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            display: "flex",
            flexDirection: "column",
            alignItems: "center",
            justifyContent: "center",
            padding: node.props?.pad != null ? Number(node.props.pad) : 8,
            gap: node.props?.gap != null ? Number(node.props.gap) : 0,
            background: hexToCSS(node.props?.color) ?? "#09090b",
            width: "100%",
            height: "100%",
            outline,
            borderRadius: 2,
          }}
        >
          {renderChildren()}
        </div>
      );
    }

    case "FlexLayout": {
      const dir = node.props?.direction === "row" ? "row" : "column";
      const align = node.props?.align === "center" ? "center"
        : node.props?.align === "right" ? "flex-end" : "flex-start";
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            display: "flex",
            flexDirection: dir,
            alignItems: dir === "column" ? align : undefined,
            justifyContent: dir === "row" ? align : undefined,
            gap: node.props?.gap != null ? Number(node.props.gap) : 0,
            width: "100%",
            outline,
            borderRadius: 2,
          }}
        >
          {renderChildren()}
        </div>
      );
    }

    case "ScrollContainer": {
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            display: "flex",
            flexDirection: "column",
            gap: node.props?.gap != null ? Number(node.props.gap) : 8,
            padding: node.props?.pad != null ? Number(node.props.pad) : 12,
            maxWidth: node.props?.maxWidth ? Number(node.props.maxWidth) : undefined,
            width: "100%",
            overflowY: "auto",
            outline,
            borderRadius: 2,
          }}
        >
          {renderChildren()}
        </div>
      );
    }

    case "Card": {
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            background: hexToCSS(node.props?.bg) ?? "#18181b",
            border: `1px solid ${hexToCSS(node.props?.border) ?? "#27272a"}`,
            borderRadius: node.props?.radius != null ? Number(node.props.radius) : 12,
            padding: node.props?.pad != null ? Number(node.props.pad) : 12,
            display: "flex",
            flexDirection: "column",
            gap: node.props?.gap != null ? Number(node.props.gap) : 8,
            width: "100%",
            outline,
          }}
        >
          {renderChildren()}
        </div>
      );
    }

    case "TitledCard": {
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            background: hexToCSS(node.props?.bg) ?? "#18181b",
            border: `1px solid ${hexToCSS(node.props?.border) ?? "#27272a"}`,
            borderRadius: 12,
            padding: 12,
            width: "100%",
            display: "flex",
            flexDirection: "column",
            gap: 4,
            outline,
          }}
        >
          <div style={{ display: "flex", alignItems: "center", justifyContent: "center", gap: 4, fontSize: 12, color: "#a1a1aa", width: "100%" }}>
            {node.props?.icon && <span className="tab-icon" style={{ fontSize: 12 }}>{String(node.props.icon)}</span>}
            <span style={{ fontWeight: 600 }}>{String(node.props?.title ?? "")}</span>
          </div>
          {renderChildren()}
        </div>
      );
    }

    case "Text": {
      // Firmware sizes: 1=10pt, 2=12pt, 3=14pt, 4=18pt, 5=24pt
      const SIZES: Record<number, number> = { 1: 10, 2: 12, 3: 14, 4: 18, 5: 24 };
      const size = SIZES[Number(node.props?.size ?? 3)] ?? 14;
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            fontSize: size,
            color: hexToCSS(node.props?.color) ?? "#fafafa",
            outline,
            borderRadius: 2,
            padding: "0px 1px",
          }}
        >
          <span className="tab-icon">{node.text ?? ""}</span>
        </div>
      );
    }

    case "GaugeCard": {
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            display: "flex",
            flexDirection: "column",
            background: "#18181b",
            border: "1px solid #27272a",
            borderRadius: 12,
            padding: 12,
            gap: 4,
            width: "100%",
            outline,
          }}
        >
          <span style={{ fontSize: 12, color: "#a1a1aa" }}>{String(node.props?.label ?? "")}</span>
          <span style={{ fontSize: 14, fontWeight: 600, color: "#fafafa" }}>{String(node.props?.value ?? "")}</span>
        </div>
      );
    }

    case "HAToggle": {
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            display: "flex",
            flexDirection: "column",
            alignItems: "center",
            padding: 8,
            gap: 4,
            width: "100%",
            outline,
            borderRadius: 2,
          }}
        >
          <span style={{ fontSize: 14, color: "#a1a1aa" }}>{node.entity ?? "toggle"}</span>
          <span style={{ fontSize: 12, color: "#ef4444" }}>&#10007; OFF</span>
        </div>
      );
    }

    case "HAWeather": {
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            display: "flex",
            flexDirection: "column",
            alignItems: "center",
            padding: 8,
            gap: 4,
            width: "100%",
            outline,
            borderRadius: 2,
          }}
        >
          <span style={{ fontSize: 14, color: "#fafafa" }}>
            <span className="tab-icon" style={{ fontSize: 14 }}>{"\uF0C2"}</span> cloudy
          </span>
          <span style={{ fontSize: 24, fontWeight: 600, color: "#fafafa" }}>--°F</span>
          <span style={{ fontSize: 12, color: "#a1a1aa" }}>
            <span className="tab-icon" style={{ fontSize: 12 }}>{"\uF043"}</span> --%
          </span>
        </div>
      );
    }

    case "HABinarySensor": {
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            display: "flex",
            justifyContent: "space-between",
            alignItems: "center",
            padding: 4,
            width: "100%",
            outline,
            borderRadius: 2,
          }}
        >
          <span style={{ fontSize: 14, color: "#fafafa" }}>{node.label || node.entity || "sensor"}</span>
          <span style={{ fontSize: 12, color: "#ef4444" }}>&#10007; Away</span>
        </div>
      );
    }

    default: {
      return (
        <div
          onClick={(e) => { e.stopPropagation(); onSelect(path); }}
          style={{
            fontSize: 12,
            color: "#71717a",
            fontStyle: "italic",
            outline,
            borderRadius: 2,
            padding: 2,
          }}
        >
          &lt;{node.type}&gt;
        </div>
      );
    }
  }
}

interface Props {
  root: ComponentNode;
  selectedPath: number[] | null;
  onSelect: (path: number[]) => void;
  boardName: string | null;
}

export default function DevicePreview({ root, selectedPath, onSelect, boardName }: Props) {
  const isRound = boardName?.includes("makerfabs") || boardName?.includes("round");
  const isLarge = boardName?.includes("waveshare") || boardName?.includes("7");

  // Render at native device resolution, then CSS scale down to fit
  const nativeW = isLarge ? 800 : 240;
  const nativeH = isLarge ? 480 : 240;
  const displayW = isLarge ? 280 : 160;
  const scale = displayW / nativeW;
  const displayH = nativeH * scale;

  return (
    <div style={{ display: "flex", flexDirection: "column", alignItems: "center", gap: 6 }}>
      <div style={{ fontSize: 11, color: "#71717a" }}>Preview</div>
      <div
        style={{
          width: displayW,
          height: displayH,
          borderRadius: isRound ? "50%" : 8,
          overflow: "hidden",
          border: "2px solid #3f3f46",
          background: "#09090b",
          flexShrink: 0,
        }}
      >
        <div
          style={{
            width: nativeW,
            height: nativeH,
            transform: `scale(${scale})`,
            transformOrigin: "top left",
            overflow: "hidden",
            padding: isRound ? "20px 24px" : 0,
          }}
        >
          <PreviewNode
            node={root}
            path={[]}
            selectedPath={selectedPath}
            onSelect={onSelect}
          />
        </div>
      </div>
    </div>
  );
}
