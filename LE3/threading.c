#include <threading.h>

void t_init()
{
        // TODO
        // instantiate current_context_idx (current context index)
        current_context_idx = 0;
        // instantiate contexts array (holds state and context information)
        for (uint8_t i = 0; i < NUM_CTX; i++) {
                contexts[i].state = INVALID;
                // idk how to instantiate of type ucontext_t
                // getcontext(&contexts[i].context);
        }


}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2)
{
        // TODO
        // search for valid index in context array
        uint8_t validIndex = 100;
        for (uint8_t i = 0; i < NUM_CTX; i++) {
                if (contexts[i].state == INVALID) {
                        validIndex = i;
                        break;
                }
        }
        // swap contexts
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
        // TODO
        // update the current context 
        getcontext(&contexts[current_context_idx].context);
        // search through contexts array for first instance of a valid context state
        uint8_t validIndex = 100;
        for (uint8_t i = 0; i < NUM_CTX; i++) {
                if (contexts[i].state == VALID) {
                        validIndex = i;
                        break;
                }
        }
        // swapcontext to that instance
        if (validIndex != 100) {
                swapcontext(&contexts[current_context_idx].context, &contexts[validIndex].context);
                current_context_idx = validIndex;

                // find total number of VALID states in context array and return
                int totalValidContexts = 0;
                for (uint8_t i = 0; i < NUM_CTX; i++) {
                        if (contexts[i].state == VALID) {
                                totalValidContexts++;
                        }
                }
                return totalValidContexts;
                
        }
        return -1;
}

void t_finish()
{
        // TODO
        // delete the context (free stack)
        free(contexts[current_context_idx].context.uc_stack.ss_sp);
        memset(&contexts[current_context_idx].context, 0, sizeof(ucontext_t));
        // set state to FINISHED
        contexts[current_context_idx].state = DONE;
}
