import { useState } from "react";
import type { ComponentNode } from "../types/manifest";
import type { SchemaMap } from "../types/schema";
import TreeNode from "./TreeNode";
import AddComponentDialog from "./AddComponentDialog";

interface Props {
  root: ComponentNode;
  selectedPath: number[] | null;
  schema: SchemaMap | null;
  onSelect: (path: number[] | null) => void;
  onMove: (path: number[], direction: "up" | "down") => void;
  onRemove: (path: number[]) => void;
  onAddChild: (parentPath: number[], type: string, position?: number) => void;
}

export default function ComponentTree({
  root,
  selectedPath,
  schema,
  onSelect,
  onMove,
  onRemove,
  onAddChild,
}: Props) {
  const [addingAt, setAddingAt] = useState<number[] | null>(null);

  const isContainer = (type: string) => schema?.[type]?.container ?? false;

  return (
    <div>
      <TreeNode
        node={root}
        path={[]}
        selectedPath={selectedPath}
        schema={schema}
        onSelect={onSelect}
        onMove={onMove}
        onRemove={onRemove}
        onAddChild={(path) => setAddingAt(path)}
        isContainer={isContainer}
        isRoot
        siblingCount={1}
        index={0}
      />

      {addingAt !== null && schema && (
        <AddComponentDialog
          schema={schema}
          onSelect={(type) => {
            onAddChild(addingAt, type);
            setAddingAt(null);
          }}
          onClose={() => setAddingAt(null)}
        />
      )}
    </div>
  );
}
