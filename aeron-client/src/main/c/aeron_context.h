/*
 * Copyright 2014-2020 Real Logic Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AERON_C_CONTEXT_H
#define AERON_C_CONTEXT_H

#include "aeronc.h"
#include "aeron_agent.h"

typedef struct aeron_context_stct
{
    char *aeron_dir;

    aeron_error_handler_t error_handler;
    void *error_handler_clientd;

    aeron_on_new_publication_t on_new_publication;
    void *on_new_publication_clientd;

    aeron_on_new_publication_t on_new_exclusive_publication;
    void *on_new_exclusive_publication_clientd;

    aeron_on_new_subscription_t on_new_subscription;
    void *on_new_subscription_clientd;

    aeron_on_available_image_t on_available_image;
    void *on_available_image_clientd;

    aeron_on_unavailable_image_t on_unavailable_image;
    void *on_unavailable_image_clientd;

    aeron_on_available_counter_t on_available_counter;
    void *on_available_counter_clientd;

    aeron_on_unavailable_counter_t on_unavailable_counter;
    void *on_unavailable_counter_clientd;

    aeron_agent_on_start_func_t agent_on_start_func;
    void *agent_on_start_state;

    aeron_on_close_client_t on_close_client;
    void *on_close_client_clientd;

    aeron_idle_strategy_func_t idle_strategy_func;
    void *idle_strategy_state;

    long media_driver_timeout_ms;
    long resource_linger_timeout_ms;

    bool use_conductor_agent_invoker;
    bool pre_touch_mapped_memory;

    aeron_clock_func_t nano_clock;
    aeron_clock_func_t epoch_clock;
}
aeron_context_t;

#endif //AERON_C_CONTEXT_H
