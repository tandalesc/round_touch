interface Props {
  boards: { name: string; screenCount: number }[];
  selectedBoard: string | null;
  onSelectBoard: (board: string) => void;
  onSave: () => void;
  saving: boolean;
  isDirty: boolean;
  mode: "tree" | "json";
  onToggleMode: () => void;
}

export default function Header({
  boards,
  selectedBoard,
  onSelectBoard,
  onSave,
  saving,
  isDirty,
  mode,
  onToggleMode,
}: Props) {
  return (
    <div className="header">
      <div className="header-brand">
        <span className="header-logo" />
        <span className="header-title">round_touch</span>
      </div>

      <div className="header-divider" />

      <div className="board-select-wrapper">
        <i className="fa fa-microchip" />
        <select
          className="board-select"
          value={selectedBoard ?? ""}
          onChange={(e) => e.target.value && onSelectBoard(e.target.value)}
        >
          <option value="">Select board...</option>
          {boards.map((b) => (
            <option key={b.name} value={b.name}>
              {b.name} ({b.screenCount})
            </option>
          ))}
        </select>
      </div>

      <div className="header-spacer" />

      {selectedBoard && (
        <>
          <div className="mode-toggle">
            <button
              className={`mode-toggle-btn${mode === "tree" ? " active" : ""}`}
              onClick={mode !== "tree" ? onToggleMode : undefined}
            >
              <i className="fa fa-sitemap" style={{ fontSize: 10 }} />
              Tree
            </button>
            <button
              className={`mode-toggle-btn${mode === "json" ? " active" : ""}`}
              onClick={mode !== "json" ? onToggleMode : undefined}
            >
              <i className="fa fa-code" style={{ fontSize: 10 }} />
              JSON
            </button>
          </div>

          {isDirty && <span className="dirty-dot" title="Unsaved changes" />}

          <button
            className="save-btn"
            onClick={onSave}
            disabled={saving || !isDirty}
          >
            <i className="fa fa-save" />
            {saving ? "Saving..." : "Save"}
          </button>
        </>
      )}
    </div>
  );
}
