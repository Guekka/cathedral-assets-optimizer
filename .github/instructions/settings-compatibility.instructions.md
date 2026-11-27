---
description: Compatibility rules for CAO settings and profile serialization.
applyTo: 'src/settings/**'
---

# Settings compatibility

CAO persists application settings and also imports/exports standalone profiles. Keep those shapes
compatible when changing serialized settings:

- Adding a field to an `NLOHMANN_DEFINE_TYPE_*` list makes it required when reading older JSON.
  Add explicit fallback/migration logic and tests for both legacy and current settings files.
- Expand and migrate before contracting: retain legacy fields while the new representation is being
  verified, and remove them only in a separately scoped migration.
- If an existing settings file is unreadable, surface the error rather than silently loading defaults
  that could later overwrite the user's saved profiles.
