enum SystemState
{
    DISARMED = 0,
    ARMED_FULL = 1,
    ARMED_PARTIAL = 2
};
extern SystemState currentSystemState = DISARMED;