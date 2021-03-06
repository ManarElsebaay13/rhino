/*
    Copyright 2018 Picovoice Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

            http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef PV_RHINO_H
#define PV_RHINO_H

#include <stdbool.h>
#include <stdint.h>

#include "picovoice.h"

#ifdef __cplusplus

extern "C"
{

#endif

/**
 * Forward declaration for speech-to-intent object (a.k.a Rhino).
 * The object directly infers intent from spoken commands within a given context of interest in real-time. It processes
 * incoming audio in consecutive frames (chunks) and at the end of each frame indicates if the intent extraction is
 * finalized. When finalized, the intent can be retrieved as structured data in the form of an intent string and pairs
 * of slots and values representing arguments (details) of the intent. The number of samples per frame can be attained
 * by calling 'pv_rhino_frame_length()'. The incoming audio needs to have a sample rate equal to 'pv_sample_rate()' and
 * be 16-bit linearly-encoded. Furthermore, Rhino operates on single-channel audio.
 */
typedef struct pv_rhino_object pv_rhino_object_t;

/**
 * Constructor.
 *
 * @param model_file_path Absolute path to file containing model parameters.
 * @param context_file_path Absolute path to file containing context parameters. A context represents the set of
 * expressions (spoken commands), intents, and intent arguments (slots) within a domain of interest.
 * @param sensitivity Sensitivity for inference. It should be a floating-point number within [0, 1]. A higher
 * sensitivity value results in fewer inference misses at the cost of potentially increasing the erroneous inference
 * rate.
 * @param[out] object Constructed speech-to-intent object.
 * @return Status code. Returns 'PV_STATUS_INVALID_ARGUMENT', 'PV_STATUS_IO_ERROR', or 'PV_STATUS_OUT_OF_MEMORY' on
 * failure.
 */
PV_API pv_status_t pv_rhino_init(
        const char *model_file_path,
        const char *context_file_path,
        float sensitivity,
        pv_rhino_object_t **object);

/**
 * Destructor.
 *
 * @param object Speech-to-intent object.
 */
PV_API void pv_rhino_delete(pv_rhino_object_t *object);

/**
 * Processes a frame of audio and emits a flag indicating if the engine has finalized intent extraction. When finalized,
 * 'pv_rhino_is_understood()' should be called to check if the spoken command is considered valid (is within the context
 * of interest).
 *
 * @param object Speech-to-intent object.
 * @param pcm A frame of audio samples. The number of samples per frame can be attained by calling
 * 'pv_rhino_frame_length()'. The incoming audio needs to have a sample rate equal to 'pv_sample_rate()' and be 16-bit
 * linearly-encoded. Furthermore, Rhino operates on single channel audio.
 * @param[out] is_finalized Flag indicating whether the engine has finalized intent extraction.
 * @return Status code. Returns 'PV_STATUS_INVALID_ARGUMENT' or 'PV_STATUS_OUT_OF_MEMORY' on failure.
 */
PV_API pv_status_t pv_rhino_process(pv_rhino_object_t *object, const int16_t *pcm, bool *is_finalized);

/**
 * Indicates if the spoken command is valid, is within the domain of interest (context), and the engine understood it.
 *
 * @param object Speech-to-intent object.
 * @param[out] is_understood Flag indicating if the spoken command is valid, is within the domain of interest (context),
 * and the engine understood it.
 * @return Status code. Returns 'PV_STATUS_INVALID_ARGUMENT' on failure.
 */
PV_API pv_status_t pv_rhino_is_understood(const pv_rhino_object_t *object, bool *is_understood);

/**
 * Getter for the intent inferred from the spoken command. The intent is presented as an intent string and pairs of slots
 * and their values. It should be called only after intent extraction is finalized and it is verified that the spoken
 * command is valid and understood via calling 'pv_rhino_is_understood()'.
 *
 * @param object Speech-to-intent object.
 * @param[out] intent Inferred intent.
 * @param[out] num_slots Number of slots.
 * @param[out] slots Array of inferred slots. Its memory needs to be freed by calling 'pv_rhino_free_slots_and_values()'.
 * @param[out] values Array of inferred slot values in the same order of inferred slots. Its memory needs to be freed
 * by calling 'pv_rhino_free_slots_and_values()'.
 * @return State code. Returns 'PV_STATUS_INVALID_ARGUMENT', 'PV_STATUS_INVALID_STATE', or 'PV_STATUS_OUT_OF_MEMORY' on
 * failure.
 */
PV_API pv_status_t pv_rhino_get_intent(
        const pv_rhino_object_t *object,
        const char **intent,
        int32_t *num_slots,
        const char ***slots,
        const char ***values);

/**
 * Frees memory resources allocated to returned slots and their corresponding values after calling
 * 'pv_rhino_get_intent()'. Note that you can't and shouldn't free these resources via standard C library 'free()'.
 *
 * @param object Speech-to-intent object.
 * @param slots Slots
 * @param values Slot values.
 *
 * @return Returns 'PV_STATUS_INVALID_ARGUMENT' on failure.
 */
PV_API pv_status_t pv_rhino_free_slots_and_values(
        const pv_rhino_object_t *object,
        const char **slots,
        const char **values);

/**
 * Resets the internal state of the engine. It should be called before the engine can be used to infer intent from a new
 * stream of audio.
 *
 * @param object Speech-to-intent object.
 * @return Status code. Returns 'PV_STATUS_INVALID_ARGUMENT' on failure.
 */
PV_API pv_status_t pv_rhino_reset(pv_rhino_object_t *object);

/**
 * Getter for context information
 *
 * @param object Speech-to-intent object.
 * @param[out] context_info Context information.
 * @return Status code. Returns 'PV_STATUS_INVALID_ARGUMENT' on failure.
 */
PV_API pv_status_t pv_rhino_context_info(const pv_rhino_object_t *object, const char **context_info);

/**
 * Getter for version string.
 *
 * @return Version.
 */
PV_API const char *pv_rhino_version(void);

/**
 * Getter for length (number of audio samples) per frame.
 *
 * @return frame length.
 */
PV_API int pv_rhino_frame_length(void);

#ifdef __cplusplus

}

#endif

#endif // PV_RHINO_H
