import { useEffect, useState } from "react";
import { useManifest } from "./hooks/useManifest";
import { fetchBoards, fetchSchema } from "./api/client";
import type { SchemaMap } from "./types/schema";
import Header from "./components/Header";
import TabManager from "./components/TabManager";
import ComponentTree from "./components/ComponentTree";
import PropEditor from "./components/PropEditor";
import RawJsonEditor from "./components/RawJsonEditor";
import DevicePreview from "./components/DevicePreview";

export default function App() {
  const m = useManifest();
  const [boards, setBoards] = useState<{ name: string; screenCount: number }[]>([]);
  const [schema, setSchema] = useState<SchemaMap | null>(null);
  const [mode, setMode] = useState<"tree" | "json">("tree");

  // Fetch boards + schema on mount
  useEffect(() => {
    fetchBoards().then(setBoards).catch(console.error);
    fetchSchema().then(setSchema).catch(console.error);
  }, []);

  // Refresh board list after save
  const handleSave = async () => {
    await m.save();
    fetchBoards().then(setBoards).catch(console.error);
  };

  // Get the selected screen root node
  const screenRoot =
    m.manifest && m.selectedScreen
      ? m.manifest.screens[m.selectedScreen] ?? null
      : null;

  // Get selected node for prop editor
  const getNodeAtPath = (
    root: typeof screenRoot,
    path: number[] | null
  ) => {
    if (!root || !path) return null;
    let node = root;
    for (const idx of path) {
      if (!node.children || idx >= node.children.length) return null;
      node = node.children[idx];
    }
    return node;
  };

  const selectedNode = getNodeAtPath(screenRoot, m.selectedPath);

  return (
    <div className="app">
      <Header
        boards={boards}
        selectedBoard={m.selectedBoard}
        onSelectBoard={m.loadManifest}
        onSave={handleSave}
        saving={m.saving}
        isDirty={m.isDirty}
        mode={mode}
        onToggleMode={() => setMode(mode === "tree" ? "json" : "tree")}
      />

      {m.error && (
        <div className="error-banner">
          {m.error}
          <button className="small" onClick={m.clearError}>
            Dismiss
          </button>
        </div>
      )}

      {!m.manifest ? (
        <div className="empty-state">
          Select a board to start editing
        </div>
      ) : mode === "json" ? (
        <RawJsonEditor
          manifest={m.manifest}
          onApply={m.replaceManifest}
        />
      ) : (
        <>
          <TabManager
            tabs={m.manifest.tabs}
            defaultScreen={m.manifest.default_screen}
            selectedScreen={m.selectedScreen}
            onSelect={m.selectScreen}
            onAdd={m.addTab}
            onRemove={m.removeTab}
            onUpdate={m.updateTab}
            onReorder={m.reorderTab}
            onSetDefault={m.setDefaultScreen}
          />

          <div className="editor-main">
            <div className="panel panel-left">
              {screenRoot ? (
                <ComponentTree
                  root={screenRoot}
                  selectedPath={m.selectedPath}
                  schema={schema}
                  onSelect={m.selectNode}
                  onMove={m.moveNode}
                  onRemove={m.removeNode}
                  onAddChild={m.addChild}
                />
              ) : (
                <div className="empty-state">No screen selected</div>
              )}
            </div>
            <div className="panel panel-right">
              {screenRoot && (
                <div style={{ marginBottom: 16, display: "flex", justifyContent: "center" }}>
                  <DevicePreview
                    root={screenRoot}
                    selectedPath={m.selectedPath}
                    onSelect={m.selectNode}
                    boardName={m.selectedBoard}
                  />
                </div>
              )}
              {selectedNode && schema ? (
                <PropEditor
                  node={selectedNode}
                  schema={schema}
                  onUpdateProps={m.updateNodeProps}
                  onUpdateField={m.updateNodeField}
                />
              ) : (
                <div className="empty-state">
                  Select a component to edit its properties
                </div>
              )}
            </div>
          </div>
        </>
      )}
    </div>
  );
}
