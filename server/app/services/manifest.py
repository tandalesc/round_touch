"""Manifest validation and CRUD operations."""

from app.database import Database
from app.schema import COMPONENT_SCHEMA, KNOWN_COMPONENT_TYPES


def validate_manifest(manifest: dict) -> list[str]:
    """Validate a manifest dict. Returns a list of error strings (empty = valid)."""
    errors = []
    if "version" not in manifest:
        errors.append("Missing 'version' key")
    if not isinstance(manifest.get("tabs"), list):
        errors.append("Missing or invalid 'tabs' (must be array)")
    if not isinstance(manifest.get("screens"), dict):
        errors.append("Missing or invalid 'screens' (must be object)")
    if "default_screen" not in manifest:
        errors.append("Missing 'default_screen' key")

    if errors:
        return errors

    tab_ids = {t["id"] for t in manifest["tabs"] if isinstance(t, dict) and "id" in t}
    screen_ids = set(manifest["screens"].keys())

    for tid in tab_ids:
        if str(tid) not in screen_ids:
            errors.append(f"Tab id={tid} has no matching screen")

    if manifest["default_screen"] not in tab_ids:
        errors.append(f"default_screen={manifest['default_screen']} is not a valid tab id")

    def check_node(node, path="root"):
        if not isinstance(node, dict):
            errors.append(f"{path}: node is not an object")
            return
        ntype = node.get("type")
        if not ntype:
            errors.append(f"{path}: missing 'type'")
            return
        if ntype not in KNOWN_COMPONENT_TYPES:
            errors.append(f"{path}: unknown component type '{ntype}'")
        schema = COMPONENT_SCHEMA.get(ntype, {})
        if not schema.get("container") and node.get("children"):
            errors.append(f"{path}: leaf component '{ntype}' cannot have children")
        for i, child in enumerate(node.get("children", [])):
            check_node(child, f"{path}.children[{i}]")

    for sid, screen in manifest["screens"].items():
        check_node(screen, f"screens[{sid}]")

    return errors
