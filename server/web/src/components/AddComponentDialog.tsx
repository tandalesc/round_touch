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
        <h3>
          <i className="fa fa-plus-circle" />
          Add Component
        </h3>

        <div className="add-dialog-group">Containers</div>
        {containers.map(([type]) => (
          <div
            key={type}
            className="add-dialog-item"
            onClick={() => onSelect(type)}
          >
            <i className="fa fa-object-group" style={{ marginRight: 8, fontSize: 11, opacity: 0.5 }} />
            {type}
          </div>
        ))}

        <div className="add-dialog-group">Components</div>
        {leaves.map(([type]) => (
          <div
            key={type}
            className="add-dialog-item"
            onClick={() => onSelect(type)}
          >
            <i className="fa fa-cube" style={{ marginRight: 8, fontSize: 11, opacity: 0.5 }} />
            {type}
          </div>
        ))}

        <div className="add-dialog-footer">
          <button onClick={onClose}>Cancel</button>
        </div>
      </div>
    </div>
  );
}
