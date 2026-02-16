import type { Manifest } from "../types/manifest";
import type { SchemaMap } from "../types/schema";

const BASE = "/api/editor";

export async function fetchBoards(): Promise<{ name: string; screenCount: number }[]> {
  const res = await fetch(`${BASE}/boards`);
  if (!res.ok) throw new Error(`Failed to fetch boards: ${res.status}`);
  const data = await res.json();
  return data.boards;
}

export async function fetchManifest(board: string): Promise<Manifest> {
  const res = await fetch(`${BASE}/manifest?board=${encodeURIComponent(board)}`);
  if (!res.ok) throw new Error(`Failed to fetch manifest: ${res.status}`);
  return res.json();
}

export async function saveManifest(
  board: string,
  manifest: Manifest
): Promise<{ ok: boolean; errors?: string[] }> {
  const res = await fetch(`${BASE}/manifest?board=${encodeURIComponent(board)}`, {
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(manifest),
  });
  if (res.status === 422) {
    const data = await res.json();
    return { ok: false, errors: data.detail?.errors ?? [data.detail] };
  }
  if (!res.ok) throw new Error(`Failed to save manifest: ${res.status}`);
  return { ok: true };
}

export async function createBoard(name: string): Promise<void> {
  const res = await fetch(`${BASE}/boards`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ name }),
  });
  if (!res.ok) {
    const data = await res.json();
    throw new Error(data.detail || `Failed to create board: ${res.status}`);
  }
}

export async function fetchSchema(): Promise<SchemaMap> {
  const res = await fetch(`${BASE}/schema`);
  if (!res.ok) throw new Error(`Failed to fetch schema: ${res.status}`);
  return res.json();
}
