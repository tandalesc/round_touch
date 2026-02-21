import { useState } from "react";
import type { Tab } from "../types/manifest";

interface Props {
  tabs: Tab[];
  defaultScreen: number;
  selectedScreen: string | null;
  onSelect: (screenId: string) => void;
  onAdd: (icon: string, label: string) => void;
  onRemove: (tabId: number) => void;
  onUpdate: (tabId: number, patch: Partial<Tab>) => void;
  onReorder: (fromIdx: number, toIdx: number) => void;
  onSetDefault: (tabId: number) => void;
}

export default function TabManager({
  tabs,
  defaultScreen,
  selectedScreen,
  onSelect,
  onAdd,
  onRemove,
  onUpdate,
  onReorder,
  onSetDefault,
}: Props) {
  const [editing, setEditing] = useState<number | null>(null);
  const [editLabel, setEditLabel] = useState("");

  const startEdit = (tab: Tab) => {
    setEditing(tab.id);
    setEditLabel(tab.label);
  };

  const commitEdit = () => {
    if (editing !== null && editLabel.trim()) {
      onUpdate(editing, { label: editLabel.trim() });
    }
    setEditing(null);
  };

  return (
    <div className="tab-manager">
      {tabs.map((tab, i) => {
        const isActive = selectedScreen === String(tab.id);
        const isDefault = tab.id === defaultScreen;

        return (
          <div
            key={tab.id}
            className={`tab-item${isActive ? " active" : ""}${isDefault ? " default" : ""}`}
            onClick={() => onSelect(String(tab.id))}
          >
            <span className="tab-icon">{tab.icon}</span>

            {editing === tab.id ? (
              <input
                autoFocus
                value={editLabel}
                onChange={(e) => setEditLabel(e.target.value)}
                onBlur={commitEdit}
                onKeyDown={(e) => {
                  if (e.key === "Enter") commitEdit();
                  if (e.key === "Escape") setEditing(null);
                }}
                onClick={(e) => e.stopPropagation()}
                style={{
                  width: "80px",
                  background: "var(--bg-input)",
                  border: "1px solid var(--accent)",
                  borderRadius: "var(--radius-sm)",
                  color: "var(--text-primary)",
                  padding: "1px 6px",
                  fontSize: "12px",
                  fontFamily: "var(--font-sans)",
                  outline: "none",
                  boxShadow: "0 0 0 2px var(--accent-subtle)",
                }}
              />
            ) : (
              <span>{tab.label}</span>
            )}

            {isActive && (
              <div className="tab-actions" onClick={(e) => e.stopPropagation()}>
                {i > 0 && (
                  <button className="icon-btn" onClick={() => onReorder(i, i - 1)} title="Move left">
                    <i className="fa fa-chevron-left" />
                  </button>
                )}
                {i < tabs.length - 1 && (
                  <button className="icon-btn" onClick={() => onReorder(i, i + 1)} title="Move right">
                    <i className="fa fa-chevron-right" />
                  </button>
                )}
                <button className="icon-btn" onClick={() => startEdit(tab)} title="Rename">
                  <i className="fa fa-pencil" />
                </button>
                {!isDefault && (
                  <button className="icon-btn" onClick={() => onSetDefault(tab.id)} title="Set as default">
                    <i className="fa fa-star-o" />
                  </button>
                )}
                {tabs.length > 1 && (
                  <button
                    className="icon-btn danger"
                    onClick={() => {
                      if (confirm(`Delete tab "${tab.label}"?`)) onRemove(tab.id);
                    }}
                    title="Delete tab"
                  >
                    <i className="fa fa-trash-o" />
                  </button>
                )}
              </div>
            )}
          </div>
        );
      })}

      <button
        className="tab-add-btn"
        onClick={() => onAdd("\uF015", "New")}
        title="Add tab"
      >
        <i className="fa fa-plus" />
      </button>
    </div>
  );
}
