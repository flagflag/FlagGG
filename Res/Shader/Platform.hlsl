
/** Avoids flow control constructs. */
#define UNROLL [unroll]
#define UNROLL_N(N) [unroll(N)]

/** Gives preference to flow control constructs. */
#define LOOP [loop]

/** Performs branching by using control flow instructions like jmp and label. */
#define BRANCH [branch]

/** Performs branching by using the cnd instructions. */
#define FLATTEN [flatten]

/** Allows a compute shader loop termination condition to be based off of a UAV read. The loop must not contain synchronization intrinsics. */
#define ALLOW_UAV_CONDITION [allow_uav_condition]
