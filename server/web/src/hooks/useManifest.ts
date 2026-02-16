import { useReducer, useCallback, useEffect } from "react";
import type { Manifest, Tab, ComponentNode } from "../types/manifest";
import * as api from "../api/client";

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

export interface ManifestState {
  manifest: Manifest | null;
  selectedBoard: string | null;
  selectedScreen: string | null;
  selectedPath: number[] | null;
  isDirty: boolean;
  error: string | null;
  saving: boolean;
}

const initialState: ManifestState = {
  manifest: null,
  selectedBoard: null,
  selectedScreen: null,
  selectedPath: null,
  isDirty: false,
  error: null,
  saving: false,
};

// ---------------------------------------------------------------------------
// Actions
// ---------------------------------------------------------------------------

type Action =
  | { type: "SET_MANIFEST"; board: string; manifest: Manifest }
  | { type: "SET_ERROR"; error: string }
  | { type: "CLEAR_ERROR" }
  | { type: "SELECT_SCREEN"; screenId: string }
  | { type: "SELECT_NODE"; path: number[] | null }
  | { type: "SAVING_START" }
  | { type: "SAVING_DONE"; error?: string }
  | { type: "MARK_CLEAN" }
  | { type: "UPDATE_MANIFEST"; manifest: Manifest };

function reducer(state: ManifestState, action: Action): ManifestState {
  switch (action.type) {
    case "SET_MANIFEST": {
      const m = action.manifest;
      const firstTab = m.tabs.length > 0 ? String(m.default_screen) : null;
      return {
        ...state,
        manifest: m,
        selectedBoard: action.board,
        selectedScreen: firstTab,
        selectedPath: null,
        isDirty: false,
        error: null,
        saving: false,
      };
    }
    case "SET_ERROR":
      return { ...state, error: action.error, saving: false };
    case "CLEAR_ERROR":
      return { ...state, error: null };
    case "SELECT_SCREEN":
      return { ...state, selectedScreen: action.screenId, selectedPath: null };
    case "SELECT_NODE":
      return { ...state, selectedPath: action.path };
    case "SAVING_START":
      return { ...state, saving: true };
    case "SAVING_DONE":
      return {
        ...state,
        saving: false,
        isDirty: action.error ? state.isDirty : false,
        error: action.error ?? null,
      };
    case "MARK_CLEAN":
      return { ...state, isDirty: false };
    case "UPDATE_MANIFEST":
      return { ...state, manifest: action.manifest, isDirty: true };
    default:
      return state;
  }
}

// ---------------------------------------------------------------------------
// Helpers for immutable tree updates
// ---------------------------------------------------------------------------

function cloneManifest(m: Manifest): Manifest {
  return JSON.parse(JSON.stringify(m));
}

function getNodeAtPath(root: ComponentNode, path: number[]): ComponentNode | null {
  let node = root;
  for (const idx of path) {
    if (!node.children || idx >= node.children.length) return null;
    node = node.children[idx];
  }
  return node;
}

function getParentAndIndex(
  root: ComponentNode,
  path: number[]
): { parent: ComponentNode; index: number } | null {
  if (path.length === 0) return null;
  const parentPath = path.slice(0, -1);
  const parent = getNodeAtPath(root, parentPath);
  if (!parent) return null;
  return { parent, index: path[path.length - 1] };
}

// ---------------------------------------------------------------------------
// Hook
// ---------------------------------------------------------------------------

export function useManifest() {
  const [state, dispatch] = useReducer(reducer, initialState);

  // Warn on unsaved changes
  useEffect(() => {
    const handler = (e: BeforeUnloadEvent) => {
      if (state.isDirty) {
        e.preventDefault();
      }
    };
    window.addEventListener("beforeunload", handler);
    return () => window.removeEventListener("beforeunload", handler);
  }, [state.isDirty]);

  const loadManifest = useCallback(async (board: string) => {
    try {
      const manifest = await api.fetchManifest(board);
      dispatch({ type: "SET_MANIFEST", board, manifest });
    } catch (e) {
      dispatch({ type: "SET_ERROR", error: (e as Error).message });
    }
  }, []);

  const save = useCallback(async () => {
    if (!state.manifest || !state.selectedBoard) return;
    dispatch({ type: "SAVING_START" });
    try {
      const result = await api.saveManifest(state.selectedBoard, state.manifest);
      if (!result.ok) {
        dispatch({
          type: "SAVING_DONE",
          error: result.errors?.join("; ") ?? "Save failed",
        });
      } else {
        dispatch({ type: "SAVING_DONE" });
      }
    } catch (e) {
      dispatch({ type: "SAVING_DONE", error: (e as Error).message });
    }
  }, [state.manifest, state.selectedBoard]);

  const selectScreen = useCallback((screenId: string) => {
    dispatch({ type: "SELECT_SCREEN", screenId });
  }, []);

  const selectNode = useCallback((path: number[] | null) => {
    dispatch({ type: "SELECT_NODE", path });
  }, []);

  const clearError = useCallback(() => {
    dispatch({ type: "CLEAR_ERROR" });
  }, []);

  // Mutate manifest (deep clone + update)
  const mutate = useCallback(
    (fn: (m: Manifest) => void) => {
      if (!state.manifest) return;
      const m = cloneManifest(state.manifest);
      fn(m);
      dispatch({ type: "UPDATE_MANIFEST", manifest: m });
    },
    [state.manifest]
  );

  // ---------------------------------------------------------------------------
  // Tab operations
  // ---------------------------------------------------------------------------

  const addTab = useCallback(
    (icon: string, label: string) => {
      mutate((m) => {
        const existingIds = m.tabs.map((t) => t.id);
        let newId = 32;
        while (existingIds.includes(newId)) newId++;
        m.tabs.push({ id: newId, icon, label });
        m.screens[String(newId)] = {
          type: "FlexLayout",
          props: { direction: "column", align: "center", gap: 12 },
          children: [{ type: "Text", props: { size: 4 }, text: `${icon} ${label}` }],
        };
      });
    },
    [mutate]
  );

  const removeTab = useCallback(
    (tabId: number) => {
      mutate((m) => {
        m.tabs = m.tabs.filter((t) => t.id !== tabId);
        delete m.screens[String(tabId)];
        if (m.default_screen === tabId && m.tabs.length > 0) {
          m.default_screen = m.tabs[0].id;
        }
      });
      if (state.selectedScreen === String(tabId)) {
        dispatch({ type: "SELECT_SCREEN", screenId: state.manifest?.tabs[0]?.id ? String(state.manifest.tabs[0].id) : "" });
      }
    },
    [mutate, state.selectedScreen, state.manifest]
  );

  const updateTab = useCallback(
    (tabId: number, patch: Partial<Tab>) => {
      mutate((m) => {
        const tab = m.tabs.find((t) => t.id === tabId);
        if (tab) Object.assign(tab, patch);
      });
    },
    [mutate]
  );

  const reorderTab = useCallback(
    (fromIdx: number, toIdx: number) => {
      mutate((m) => {
        const [tab] = m.tabs.splice(fromIdx, 1);
        m.tabs.splice(toIdx, 0, tab);
      });
    },
    [mutate]
  );

  const setDefaultScreen = useCallback(
    (tabId: number) => {
      mutate((m) => {
        m.default_screen = tabId;
      });
    },
    [mutate]
  );

  // ---------------------------------------------------------------------------
  // Tree operations
  // ---------------------------------------------------------------------------

  const updateNodeProps = useCallback(
    (key: string, value: unknown) => {
      if (!state.selectedScreen || !state.selectedPath) return;
      mutate((m) => {
        const root = m.screens[state.selectedScreen!];
        const node = getNodeAtPath(root, state.selectedPath!);
        if (!node) return;
        if (!node.props) node.props = {};
        if (value === null || value === undefined || value === "") {
          delete node.props[key];
          if (Object.keys(node.props).length === 0) delete node.props;
        } else {
          node.props[key] = value;
        }
      });
    },
    [mutate, state.selectedScreen, state.selectedPath]
  );

  const updateNodeField = useCallback(
    (field: string, value: unknown) => {
      if (!state.selectedScreen || !state.selectedPath) return;
      mutate((m) => {
        const root = m.screens[state.selectedScreen!];
        const node = getNodeAtPath(root, state.selectedPath!);
        if (!node) return;
        if (value === null || value === undefined || value === "") {
          delete (node as unknown as Record<string, unknown>)[field];
        } else {
          (node as unknown as Record<string, unknown>)[field] = value;
        }
      });
    },
    [mutate, state.selectedScreen, state.selectedPath]
  );

  const addChild = useCallback(
    (parentPath: number[], componentType: string, position?: number) => {
      if (!state.selectedScreen) return;
      const newNode: ComponentNode = { type: componentType };
      mutate((m) => {
        const root = m.screens[state.selectedScreen!];
        const parent = getNodeAtPath(root, parentPath);
        if (!parent) return;
        if (!parent.children) parent.children = [];
        const idx = position ?? parent.children.length;
        parent.children.splice(idx, 0, newNode);
      });
    },
    [mutate, state.selectedScreen]
  );

  const removeNode = useCallback(
    (path: number[]) => {
      if (!state.selectedScreen || path.length === 0) return;
      mutate((m) => {
        const root = m.screens[state.selectedScreen!];
        const pi = getParentAndIndex(root, path);
        if (!pi || !pi.parent.children) return;
        pi.parent.children.splice(pi.index, 1);
        if (pi.parent.children.length === 0) delete pi.parent.children;
      });
      // Deselect if the removed node was selected
      if (
        state.selectedPath &&
        JSON.stringify(state.selectedPath) === JSON.stringify(path)
      ) {
        dispatch({ type: "SELECT_NODE", path: null });
      }
    },
    [mutate, state.selectedScreen, state.selectedPath]
  );

  const moveNode = useCallback(
    (path: number[], direction: "up" | "down") => {
      if (!state.selectedScreen || path.length === 0) return;
      mutate((m) => {
        const root = m.screens[state.selectedScreen!];
        const pi = getParentAndIndex(root, path);
        if (!pi || !pi.parent.children) return;
        const newIdx = direction === "up" ? pi.index - 1 : pi.index + 1;
        if (newIdx < 0 || newIdx >= pi.parent.children.length) return;
        const [node] = pi.parent.children.splice(pi.index, 1);
        pi.parent.children.splice(newIdx, 0, node);
      });
      // Update selectedPath to follow the moved node
      if (
        state.selectedPath &&
        JSON.stringify(state.selectedPath) === JSON.stringify(path)
      ) {
        const lastIdx = path[path.length - 1];
        const newIdx = direction === "up" ? lastIdx - 1 : lastIdx + 1;
        const newPath = [...path.slice(0, -1), newIdx];
        dispatch({ type: "SELECT_NODE", path: newPath });
      }
    },
    [mutate, state.selectedScreen, state.selectedPath]
  );

  const replaceManifest = useCallback(
    (manifest: Manifest) => {
      dispatch({ type: "UPDATE_MANIFEST", manifest });
    },
    []
  );

  return {
    ...state,
    loadManifest,
    save,
    selectScreen,
    selectNode,
    clearError,
    addTab,
    removeTab,
    updateTab,
    reorderTab,
    setDefaultScreen,
    updateNodeProps,
    updateNodeField,
    addChild,
    removeNode,
    moveNode,
    replaceManifest,
  };
}

export type ManifestActions = ReturnType<typeof useManifest>;
