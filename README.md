# edgez-rf-api

Vendor-neutral C interfaces between the EdgeZ UMAC and radio backends.

Upper layers depend only on this repository. Vendor adapters, including
`edgez-rf-morsemicro`, implement `edgez_rf_backend_ops` and the generic binding
entry points without exposing vendor packet or driver types to the UMAC.

The public headers cover the radio backend contract, runtime status API, and
the optional mesh/Hostap event bridge. The API is licensed under Apache-2.0.
