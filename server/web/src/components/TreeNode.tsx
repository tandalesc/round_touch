import { useState } from "react";
import type { ComponentNode } from "../types/manifest";
import type { SchemaMap } from "../types/schema";

interface Props {
  node: ComponentNode;
  path: number[];
  selectedPath: number[] | null;
  schema: SchemaMap | null;
  onSelect: (path: number[] | null) => void;
  onMove: (path: number[], direction: "up" | "down") => void;
  onRemove: (path: number[]) => void;
  onAddChild: (parentPath: number[]) => void;
  isContainer: (type: string) => boolean;
  isRoot?: boolean;
  siblingCount: number;
  index: number;
}

function nodeSummary(node: ComponentNode): string {
  if (node.text) return `"${node.text.slice(0, 30)}"`;
  if (node.entity) return node.entity;
  if (node.label) return node.label;
  if (node.props?.label) return String(node.props.label);
  if (node.props?.title) return String(node.props.title);
  return "";
}

export default function TreeNode({
  node,
  path,
  selectedPath,
  schema,
  onSelect,
  onMove,
  onRemove,
  onAddChild,
  isContainer,
  isRoot,
  siblingCount,
  index,
}: Props) {
  const [expanded, setExpanded] = useState(true);
  const hasChildren = node.children && node.children.length > 0;
  const canHaveChildren = isContainer(node.type);
  const isSelected =
    selectedPath !== null &&
    path.length === selectedPath.length &&
    path.every((v, i) => v === selectedPath[i]);

  const summary = nodeSummary(node);

  return (
    <div className="tree-node">
      <div
        className={`tree-node-header${isSelected ? " selected" : ""}`}
        onClick={() => onSelect(path)}
      >
        <span
          className="tree-node-toggle"
          onClick={(e) => {
            e.stopPropagation();
            if (hasChildren || canHaveChildren) setExpanded(!expanded);
          }}
        >
          {hasChildren ? (expanded ? "\u25BE" : "\u25B8") : canHaveChildren ? "\u25B8" : "\u00B7"}
        </span>
        <span className="tree-node-type">{node.type}</span>
        {summary && <span className="tree-node-summary">{summary}</span>}
        <div className="tree-node-actions">
          {!isRoot && index > 0 && (
            <button
              className="icon-btn"
              onClick={(e) => { e.stopPropagation(); onMove(path, "up"); }}
              title="Move up"
            >
              <i className="fa fa-arrow-up" />
            </button>
          )}
          {!isRoot && index < siblingCount - 1 && (
            <button
              className="icon-btn"
              onClick={(e) => { e.stopPropagation(); onMove(path, "down"); }}
              title="Move down"
            >
              <i className="fa fa-arrow-down" />
            </button>
          )}
          {canHaveChildren && (
            <button
              className="icon-btn"
              onClick={(e) => { e.stopPropagation(); onAddChild(path); }}
              title="Add child"
            >
              <i className="fa fa-plus" />
            </button>
          )}
          {!isRoot && (
            <button
              className="icon-btn danger"
              onClick={(e) => {
                e.stopPropagation();
                const childCount = node.children?.length ?? 0;
                if (childCount > 0 && !confirm(`Delete "${node.type}" and its ${childCount} children?`)) return;
                onRemove(path);
              }}
              title="Delete"
            >
              <i className="fa fa-trash-o" />
            </button>
          )}
        </div>
      </div>

      {expanded && hasChildren && (
        <div className="tree-node-children">
          {node.children!.map((child, i) => (
            <TreeNode
              key={i}
              node={child}
              path={[...path, i]}
              selectedPath={selectedPath}
              schema={schema}
              onSelect={onSelect}
              onMove={onMove}
              onRemove={onRemove}
              onAddChild={onAddChild}
              isContainer={isContainer}
              siblingCount={node.children!.length}
              index={i}
            />
          ))}
        </div>
      )}
    </div>
  );
}
