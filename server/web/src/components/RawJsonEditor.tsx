import { useState, useEffect } from "react";
import type { Manifest } from "../types/manifest";

interface Props {
  manifest: Manifest;
  onApply: (manifest: Manifest) => void;
}

export default function RawJsonEditor({ manifest, onApply }: Props) {
  const [text, setText] = useState("");
  const [error, setError] = useState<string | null>(null);
  const [localDirty, setLocalDirty] = useState(false);

  useEffect(() => {
    setText(JSON.stringify(manifest, null, 2));
    setError(null);
    setLocalDirty(false);
  }, [manifest]);

  const handleApply = () => {
    try {
      const parsed = JSON.parse(text);
      if (!parsed.version || !parsed.tabs || !parsed.screens) {
        setError("Missing required keys: version, tabs, screens");
        return;
      }
      setError(null);
      setLocalDirty(false);
      onApply(parsed as Manifest);
    } catch (e) {
      setError((e as Error).message);
    }
  };

  const handleFormat = () => {
    try {
      const parsed = JSON.parse(text);
      setText(JSON.stringify(parsed, null, 2));
      setError(null);
    } catch (e) {
      setError((e as Error).message);
    }
  };

  return (
    <div className="json-editor">
      <textarea
        value={text}
        onChange={(e) => {
          setText(e.target.value);
          setLocalDirty(true);
          setError(null);
        }}
        spellCheck={false}
      />
      <div className="json-editor-actions">
        <button onClick={handleFormat}>
          <i className="fa fa-indent" style={{ fontSize: 11 }} />
          Format
        </button>
        <button
          className="primary"
          onClick={handleApply}
          disabled={!localDirty}
        >
          <i className="fa fa-check" style={{ fontSize: 11 }} />
          Apply to Editor
        </button>
        {error && (
          <span className="json-editor-error">
            <i className="fa fa-exclamation-triangle" />
            {error}
          </span>
        )}
      </div>
    </div>
  );
}
