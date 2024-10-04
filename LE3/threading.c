#include <threading.h>
#include <stdio.h>
void t_init()
{
        // instantiate current_context_idx (current context index)
        current_context_idx = 0;
        // instantiate contexts array (holds state and context information)
        for (uint8_t i = 0; i < NUM_CTX; i++) {
                contexts[i].state = INVALID;
        }
        // contexts[0] will hold the main context
        contexts[0].state = VALID;
        getcontext(&contexts[0].context);
}


int32_t t_create(fptr foo, int32_t arg1, int32_t arg2)
{
        // search for valid index in context array
        uint8_t validIndex = 100;
        for (uint8_t i = 0; i < NUM_CTX; i++) {
                if (contexts[i].state == INVALID) {
                        validIndex = i;
                        break;
                }
        }
        // make context
        if (validIndex != 100) {
                // initialize context at current location, create stack, set variables
                getcontext(&contexts[validIndex].context);
                contexts[validIndex].context.uc_stack.ss_sp = (char *) malloc(STK_SZ);
                contexts[validIndex].context.uc_stack.ss_size = STK_SZ;
                contexts[validIndex].context.uc_stack.ss_flags = 0;
                contexts[validIndex].context.uc_link = NULL;
                
                makecontext(&contexts[validIndex].context, (void (*)()) foo, 2, arg1, arg2);

                contexts[validIndex].state = VALID;
                return 0;
        }
        return 1;
} // end t_create()


int32_t t_yield()
{
        // search through contexts array for first instance of a valid context state
        uint8_t validIndex = 100;
        // for (uint8_t i = NUM_CTX-1; i > 0; i--) {   // this works. do not delete
        for (uint8_t i = current_context_idx+1; i != current_context_idx; i = (uint8_t)((i+1) % NUM_CTX)) {
                // if (contexts[i].state == VALID && i != current_context_idx) {
                if (contexts[i].state == VALID && i != current_context_idx) {
                        validIndex = i;
                        break;
                }
        }
        // swap to valid context
        if (validIndex != 100) {
                // update current context index before swapping contexts
                uint8_t pastCurIdx = current_context_idx;
                current_context_idx = validIndex;

                swapcontext(&contexts[pastCurIdx].context, &contexts[validIndex].context);
                // after returning from swap, find total number of VALID states remaining in context array and return value-1
                //      the -1 is because the main context will always be valid and should not be counted as a worker
                int totalValidContexts = 0;
                for (uint8_t i = 0; i < NUM_CTX; i++) {
                        if (contexts[i].state == VALID) {
                                totalValidContexts++;
                        }
                }
                return totalValidContexts-1;
                
        }
        // no valid context found
        return -1;
} // end t_yield()


void t_finish()
{
        // delete the current context (free stack and memset to 0)
        free(contexts[current_context_idx].context.uc_stack.ss_sp);
        memset(&contexts[current_context_idx].context, 0, sizeof(ucontext_t));
        // set state to FINISHED
        contexts[current_context_idx].state = DONE;
        // yield so that other workers can finish
        t_yield();
}
