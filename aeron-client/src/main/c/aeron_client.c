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

#if defined(__linux__)
#define _BSD_SOURCE
#define _GNU_SOURCE
#ifdef HAVE_BSDSTDLIB_H
#include <bsd/stdlib.h>
#endif
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "aeronc.h"
#include "aeron_client.h"
#include "util/aeron_error.h"
#include "aeron_alloc.h"
#include "aeron_context.h"

inline static int aeron_do_work(void *clientd)
{
    aeron_t *client = (aeron_t *)clientd;
    return aeron_client_conductor_do_work(&client->conductor);
}

inline static void aeron_on_close(void *clientd)
{
    aeron_t *client = (aeron_t *)clientd;

    aeron_client_conductor_on_close(&client->conductor);
}

int aeron_init(aeron_t **client, aeron_context_t *context)
{
    aeron_t *_client = NULL;

    if (NULL == client || NULL == context)
    {
        errno = EINVAL;
        aeron_set_err(EINVAL, "aeron_init: %s", strerror(EINVAL));
        goto error;
    }

    if (aeron_alloc((void **)&_client, sizeof(aeron_t)) < 0)
    {
        aeron_set_err_from_last_err_code("%s:%d", __FILE__, __LINE__);
        goto error;
    }

    _client->context = context;

    _client->runner.agent_state = AERON_AGENT_STATE_UNUSED;
    _client->runner.role_name = NULL;
    _client->runner.on_close = NULL;

    if (aeron_client_conductor_init(&_client->conductor, context) < 0)
    {
        goto error;
    }

    if (aeron_agent_init(
        &_client->runner,
        "[aeron-client-conductor]",
        _client,
        _client->context->agent_on_start_func,
        _client->context->agent_on_start_state,
        aeron_do_work,
        aeron_on_close,
        _client->context->idle_strategy_func,
        _client->context->idle_strategy_state) < 0)
    {
        goto error;
    }

    *client = _client;
    return 0;

    error:

    if (NULL != _client)
    {
        aeron_free(_client);
    }

    return -1;
}

int aeron_start(aeron_t *client)
{

    if (NULL == client)
    {
        errno = EINVAL;
        aeron_set_err(EINVAL, "aeron_start: %s", strerror(EINVAL));
        return -1;
    }

    if (!client->context->use_conductor_agent_invoker)
    {
        if (aeron_agent_start(&client->runner) < 0)
        {
            return -1;
        }
    }
    else
    {
        if (NULL != client->runner.on_start)
        {
            client->runner.on_start(client->runner.on_start_state, client->runner.role_name);
        }

        client->runner.state = AERON_AGENT_STATE_MANUAL;
    }

    return 0;
}

int aeron_main_do_work(aeron_t *client)
{
    if (NULL == client || !client->context->use_conductor_agent_invoker)
    {
        errno = EINVAL;
        aeron_set_err(EINVAL, "aeron_dmain_do_work: %s", strerror(EINVAL));
        return -1;
    }

    return aeron_agent_do_work(&client->runner);
}

void aeron_main_idle_strategy(aeron_t *client, int work_count)
{
    if (NULL != client)
    {
        aeron_agent_idle(&client->runner, work_count);
    }
}

int aeron_close(aeron_t *client)
{
    if (NULL != client)
    {
        if (aeron_agent_stop(&client->runner) < 0)
        {
            return -1;
        }

        if (aeron_agent_close(&client->runner) < 0)
        {
            return -1;
        }

        aeron_free(client);
    }

    return 0;
}
