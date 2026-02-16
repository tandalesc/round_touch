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
      <span className="header-title">round_touch</span>

      <select
        value={selectedBoard ?? ""}
        onChange={(e) => e.target.value && onSelectBoard(e.target.value)}
      >
        <option value="">Select board...</option>
        {boards.map((b) => (
          <option key={b.name} value={b.name}>
            {b.name} ({b.screenCount} screens)
          </option>
        ))}
      </select>

      {selectedBoard && (
        <>
          <button
            className={mode === "tree" ? "active" : ""}
            onClick={mode !== "tree" ? onToggleMode : undefined}
          >
            Tree
          </button>
          <button
            className={mode === "json" ? "active" : ""}
            onClick={mode !== "json" ? onToggleMode : undefined}
          >
            JSON
          </button>

          <button
            className="primary"
            onClick={onSave}
            disabled={saving || !isDirty}
          >
            {saving ? "Saving..." : "Save"}
          </button>
          {isDirty && <span className="dirty-dot" title="Unsaved changes" />}
        </>
      )}
    </div>
  );
}
