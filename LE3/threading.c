#include <threading.h>
#include <stdio.h>
void t_init()
{
        // printf("t_init called\n");
        // TODO

        // instantiate current_context_idx (current context index)
        current_context_idx = 0;
        // instantiate contexts array (holds state and context information)
        for (uint8_t i = 0; i < NUM_CTX; i++) {
                contexts[i].state = INVALID;
                // idk how to instantiate of type ucontext_t
                // getcontext(&contexts[i].context);
        }
        // if im not wrong, contexts[0] should hold the main function context information
        contexts[0].state = VALID;
        getcontext(&contexts[0].context);
}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2)
{
        // printf("t_create called\n");
        // TODO
        // search for valid index in context array
        uint8_t validIndex = 100;
        for (uint8_t i = 0; i < NUM_CTX; i++) {
                if (contexts[i].state == INVALID) {
                        validIndex = i;
                        break;
                }
        }
        // printf("context put at index: %d\n", validIndex);
        // make context
        if (validIndex != 100) {
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
        

}

int32_t t_yield()
{
        // printf("t_yield called\n");
        // TODO
        // update the current context 
        // getcontext(&contexts[current_context_idx].context);
        // printf("context got\n");
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
        // printf("first loop done. validIndex is: %d\n", validIndex);
        // swapcontext to that context
        if (validIndex != 100) {
                uint8_t pastCurIdx = current_context_idx;
                current_context_idx = validIndex;
                // printf("current_context_idx is now: %d\n", current_context_idx);
                swapcontext(&contexts[pastCurIdx].context, &contexts[validIndex].context);
                // current_context_idx = validIndex;
                // printf("context swapped. current_context_idk is now: %d\n", current_context_idx);

                // find total number of VALID states in context array and return
                int totalValidContexts = 0;
                for (uint8_t i = 0; i < NUM_CTX; i++) {
                        if (contexts[i].state == VALID) {
                                totalValidContexts++;
                        }
                }
                // printf("valid contexts counted\n");
                return totalValidContexts-1;
                
        }
        return -1;
}

void t_finish()
{
        // printf("t_finish called\n");
        // TODO
        // delete the context (free stack)
        free(contexts[current_context_idx].context.uc_stack.ss_sp);
        memset(&contexts[current_context_idx].context, 0, sizeof(ucontext_t));
        // set state to FINISHED
        contexts[current_context_idx].state = DONE;
        // yield so that other workers can finish
        t_yield();
}
