export interface ComponentNode {
  type: string;
  props?: Record<string, unknown>;
  text?: string;
  entity?: string;
  label?: string;
  children?: ComponentNode[];
}

export interface Tab {
  id: number;
  icon: string;
  label: string;
}

export interface Manifest {
  version: number;
  default_screen: number;
  tabs: Tab[];
  screens: Record<string, ComponentNode>;
}
