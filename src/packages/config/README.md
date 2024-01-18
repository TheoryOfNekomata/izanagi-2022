## Procedure

Subsystem calls its initialize method.

1. Subsystem copies hardcoded default values to its in-memory state container.
2. Subsystem defines the available config items it has, and how it is connected to various config sources (i.e. specify its section and key in the config file, as well as supplying command line options connected to this config item).
3. Subsystem binds the config items with their respective sections and keys in the config file (i.e. correctly point the values to the respective internal state container of the app).
4. Subsystem retrieves the config file values as well as command line arguments (higher priority) supplied to the app upon invocation.
5. Subsystem checks if there are some volatile state data to retrieve and syncs it with the state.
6. Subsystem saves the state to the config file.
