/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * @file logger.c
 *
 * @brief Logger implementation
 * */

#include "logger/logger.h"
#include "logger/stream.h"

#include "logger_list.h"
#include "logger_assert.h"
#include "logger_memory.h"
#include "logger_stream_message.h"

#include <safe-string/safe_lib.h>
#include <stdio.h>
#include <string.h>

static const union logger_options g_logger_default_options = {
    .option = {
        .level = LOG_DEBUG,
        .time_format = LOG_TIME_DATE_NS,
        .color = false,
        .tagging = true,
        .more_debug = true,
        .output_enable = true
    }
};

/*!
 * @struct logger
 * @brief Logger object
 *
 * @var logger::tag
 * Tag string for log message
 *
 * @var logger::stream_list
 * List of logger stream objects which to write log message
 *
 * @var logger::options
 * Control options for logger how to handle log message
 * */
struct logger {
    const char *tag;
    struct logger_list stream_list;
    union logger_options options;
};

static void __log_write(struct logger *inst,
        const unsigned int level,
        const char *file_name,
        const char *function_name,
        const unsigned int line_number,
        struct logger_stream_message *msg,
        const size_t size) {
    int err;

    /* Additional debug information */
    if (true == inst->options.option.more_debug) {
        /* We don't need to allocate memory for string, because constant string
         * are located in read-only section by the compiler/linker and they
         * have non-volatile constant address that will never change during
         * whole program execution */
        msg->line_number = line_number;
        msg->file_name = (NULL == file_name) ? "" : file_name;
        msg->function_name = (NULL == function_name) ? "" : function_name;
    }

    /* Set log level, tag, flags and time stamp format */
    msg->tag = inst->tag;
    msg->options.raw = inst->options.raw;
    msg->options.option.level = LOG_LEVEL_MASK & level;

    /* Re-stamp log time */
    logger_time_update(&msg->log_time);

    struct logger_list_node *it;
    struct logger_stream_message *msg_copy = NULL;

    /* When more than one stream object present in the list,
     * processing other stream objects with new copy message, because
     * other streams can handle message with different speed */
    for (it = inst->stream_list.first; NULL != it; it = it->next) {
        if (NULL != it->next) {
            msg_copy = logger_memory_alloc(size);
            if (NULL != msg_copy) {
                memcpy_s(msg_copy, size, msg, size);
            } else {
                logger_memory_free(msg);
                return;
            }
        }

        err = logger_stream_add_message(it->object,
                msg, LOGGER_MESSAGE_STREAM_WRITE);
        if (err) {
            logger_memory_free(msg);
            logger_memory_free(msg_copy);
            return;
        }

        msg = msg_copy;
        msg_copy = NULL;
    }
}

LOGGER_PRINTF_FORMAT(6, 0)
static void __vlog_write(struct logger *inst,
        const unsigned int level,
        const char *file_name,
        const char *function_name,
        const unsigned int line_number,
        const char *fmt, va_list args) {

    if (false == inst->options.option.output_enable) return;
    if (inst->options.option.level < level) return;

    va_list args_copy;
    va_copy(args_copy, args);

    /* Get how much bytes to allocate for message string */
    int message_length = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    if (message_length < 0) return;

    /* Create logger message object extended with message string */
    size_t size =  sizeof(struct logger_stream_message)
        + (size_t)message_length + 1;
    struct logger_stream_message *msg = logger_memory_alloc(size);
    if (NULL == msg) return;

    /* Put formatted string to logger message object */
    if (vsnprintf(msg->message, (size_t)message_length, fmt, args) < 0) {
        logger_memory_free(msg);
        return;
    }

    __log_write(inst, level, file_name, function_name, line_number, msg, size);
 }

void _log_write(struct logger *inst, const unsigned int level,
        const char *file_name,
        const char *function_name,
        const unsigned int line_number,
        const char *message) {
    logger_assert(NULL != inst);

    size_t message_length = strnlen_s(message, RSIZE_MAX_STR);
    size_t size =  sizeof(struct logger_stream_message) + message_length + 1;

    struct logger_stream_message *msg = logger_memory_alloc(size);
    if (NULL == msg) {
        return;
    }

    memcpy_s(msg->message, message_length, message, message_length);
    msg->message[message_length] = '\0';

    __log_write(inst, level, file_name, function_name, line_number, msg, size);
}

LOGGER_PRINTF_FORMAT(6, 0)
void _log_vwrite(struct logger *inst, const unsigned int level,
        const char *file_name,
        const char *function_name,
        const unsigned int line_number,
        const char *fmt, va_list args) {
    logger_assert(NULL != inst);
    logger_assert(NULL != fmt);

    __vlog_write(inst, level, file_name, function_name, line_number, fmt, args);
}

LOGGER_PRINTF_FORMAT(6, 0)
void _log_fwrite(struct logger *inst, const unsigned int level,
        const char *file_name,
        const char *function_name,
        const unsigned int line_number,
        const char *fmt, ...) {
    logger_assert(NULL != inst);
    logger_assert(NULL != fmt);

    va_list args;
    va_start(args, fmt);

    __vlog_write(inst, level, file_name, function_name, line_number, fmt, args);

    va_end(args);
}

struct logger *logger_create(const char *tag, union logger_options *options) {
    /* Default values */
    struct logger *inst = logger_memory_alloc(sizeof(struct logger));

    if (NULL != inst) {
        memset(inst, 0, sizeof(struct logger));
        if (NULL != options) {
            inst->options.raw = options->raw;
        } else {
            inst->options.raw = g_logger_default_options.raw;
        }
        inst->tag = tag;
    }

    return inst;
}

void logger_destroy(struct logger *inst) {
    if (NULL != inst) {
        logger_list_clear(&inst->stream_list);
        logger_memory_free(inst);
    }
}

void logger_add_stream(struct logger *inst, struct logger_stream *stream) {
    logger_assert(NULL != inst);
    logger_assert(NULL != stream);

    if (true != logger_list_exist(&inst->stream_list, stream)) {
        logger_list_push(&inst->stream_list, stream, 0);
    }
}

void logger_remove_stream(struct logger *inst, struct logger_stream *stream) {
    logger_assert(NULL != inst);
    logger_assert(NULL != stream);

    struct logger_list_node *it = NULL;

    for (it = inst->stream_list.first; it != NULL; it = it->next) {
        if (it->object == stream) {
            logger_list_remove_node(&inst->stream_list, it);
            break;
        }
    }
}

void logger_set_options(struct logger *inst, union logger_options *options) {
    logger_assert(NULL != inst);

    if (NULL != options) {
        inst->options.raw = options->raw;
    } else {
        inst->options.raw = g_logger_default_options.raw;
    }
}

void logger_get_options(struct logger *inst, union logger_options *options) {
    logger_assert(NULL != inst);

    if (NULL != options) {
        options->raw = inst->options.raw;
    }
}
