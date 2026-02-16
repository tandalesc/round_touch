import type { ComponentNode } from "../types/manifest";
import type { SchemaMap, PropDef } from "../types/schema";
import ColorPicker from "./ColorPicker";
import IconPicker from "./IconPicker";

interface Props {
  node: ComponentNode;
  schema: SchemaMap;
  onUpdateProps: (key: string, value: unknown) => void;
  onUpdateField: (field: string, value: unknown) => void;
}

function PropField({
  def,
  value,
  onChange,
}: {
  def: PropDef;
  value: unknown;
  onChange: (value: unknown) => void;
}) {
  if (def.type === "color") {
    return (
      <div className="prop-field">
        <label>{def.label}</label>
        <ColorPicker
          value={(value as string) ?? ""}
          onChange={(v) => onChange(v || null)}
        />
      </div>
    );
  }

  if (def.type === "icon") {
    return (
      <div className="prop-field">
        <label>{def.label}</label>
        <IconPicker
          value={(value as string) ?? ""}
          onChange={onChange}
        />
      </div>
    );
  }

  if (def.type === "enum") {
    return (
      <div className="prop-field">
        <label>{def.label}</label>
        <select
          value={(value as string) ?? def.default ?? ""}
          onChange={(e) => onChange(e.target.value || null)}
        >
          {(def.options ?? []).map((opt) => (
            <option key={opt} value={opt}>
              {opt}
            </option>
          ))}
        </select>
      </div>
    );
  }

  if (def.type === "int") {
    return (
      <div className="prop-field">
        <label>{def.label}</label>
        <input
          type="number"
          value={value != null ? String(value) : ""}
          min={def.min}
          max={def.max}
          placeholder={def.default != null ? String(def.default) : ""}
          onChange={(e) => {
            const v = e.target.value;
            onChange(v === "" ? null : Number(v));
          }}
        />
      </div>
    );
  }

  // string
  return (
    <div className="prop-field">
      <label>{def.label}</label>
      <input
        type="text"
        value={(value as string) ?? ""}
        placeholder={def.default != null ? String(def.default) : ""}
        onChange={(e) => onChange(e.target.value)}
      />
    </div>
  );
}

export default function PropEditor({
  node,
  schema,
  onUpdateProps,
  onUpdateField,
}: Props) {
  const cs = schema[node.type];
  if (!cs) {
    return (
      <div className="prop-editor">
        <h3>{node.type}</h3>
        <div style={{ color: "var(--text-dim)" }}>Unknown component type</div>
      </div>
    );
  }

  const fieldEntries = Object.entries(cs.fields || {});
  const propEntries = Object.entries(cs.props || {});

  return (
    <div className="prop-editor">
      <h3>{node.type}</h3>

      {fieldEntries.length > 0 && (
        <>
          <div className="prop-editor-section">Fields</div>
          {fieldEntries.map(([key, def]) => (
            <PropField
              key={key}
              def={def}
              value={(node as unknown as Record<string, unknown>)[key]}
              onChange={(v) => onUpdateField(key, v)}
            />
          ))}
        </>
      )}

      {propEntries.length > 0 && (
        <>
          <div className="prop-editor-section">Props</div>
          {propEntries.map(([key, def]) => (
            <PropField
              key={key}
              def={def}
              value={node.props?.[key]}
              onChange={(v) => onUpdateProps(key, v)}
            />
          ))}
        </>
      )}

      {fieldEntries.length === 0 && propEntries.length === 0 && (
        <div style={{ color: "var(--text-dim)" }}>No editable properties</div>
      )}
    </div>
  );
}
