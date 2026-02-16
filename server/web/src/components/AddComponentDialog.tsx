import type { SchemaMap } from "../types/schema";

interface Props {
  schema: SchemaMap;
  onSelect: (type: string) => void;
  onClose: () => void;
}

export default function AddComponentDialog({ schema, onSelect, onClose }: Props) {
  const containers = Object.entries(schema).filter(([, s]) => s.container);
  const leaves = Object.entries(schema).filter(([, s]) => !s.container);

  return (
    <div className="add-dialog-overlay" onClick={onClose}>
      <div className="add-dialog" onClick={(e) => e.stopPropagation()}>
        <h3>Add Component</h3>

        <div className="add-dialog-group">Containers</div>
        {containers.map(([type]) => (
          <div
            key={type}
            className="add-dialog-item"
            onClick={() => onSelect(type)}
          >
            {type}
          </div>
        ))}

        <div className="add-dialog-group">Leaves</div>
        {leaves.map(([type]) => (
          <div
            key={type}
            className="add-dialog-item"
            onClick={() => onSelect(type)}
          >
            {type}
          </div>
        ))}

        <div style={{ marginTop: 12, textAlign: "right" }}>
          <button onClick={onClose}>Cancel</button>
        </div>
      </div>
    </div>
  );
}
