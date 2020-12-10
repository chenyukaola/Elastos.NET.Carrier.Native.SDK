/*
 * Copyright (c) 2018 Elastos Foundation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __ELASTOS_CARRIER_SESSION_H__
#define __ELASTOS_CARRIER_SESSION_H__

#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

#include <carrier.h>

#if defined(__APPLE__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CARRIER_STATIC)
#define CARRIER_API
#elif defined(CARRIER_DYNAMIC)
  #ifdef CARRIER_BUILD
    #if defined(_WIN32) || defined(_WIN64)
      #define CARRIER_API        __declspec(dllexport)
    #else
      #define CARRIER_API        __attribute__((visibility("default")))
    #endif
  #else
    #if defined(_WIN32) || defined(_WIN64)
      #define CARRIER_API        __declspec(dllimport)
    #else
      #define CARRIER_API        __attribute__((visibility("default")))
    #endif
  #endif
#else
#define CARRIER_API
#endif

#define CARRIER_MAX_IP_STRING_LEN   45

#define CARRIER_MAX_USER_DATA_LEN   2048

typedef struct CarrierSession CarrierSession;
typedef ptrdiff_t   ssize_t;

/**
 * \~English
 * Carrier stream types definition.
 * Reference:
 *      https://tools.ietf.org/html/rfc4566#section-5.14
 *      https://tools.ietf.org/html/rfc4566#section-8
 *
 */
typedef enum CarrierStreamType {
    /**
     * \~English
     *  Audio stream.
     */
    CarrierStreamType_audio = 0,
    /**
     * \~English
     *  Video stream.
     */
    CarrierStreamType_video,
    /**
     * \~English
     *  Text stream.
     */
    CarrierStreamType_text,
    /**
     * \~English
     *  Application stream.
     */
    CarrierStreamType_application,
    /**
     * \~English
     *  Message stream.
     */
    CarrierStreamType_message
} CarrierStreamType;

/**
 * \~English
 * Stream address type.
 */
typedef enum CandidateType {
    /**
     * \~English
     * The address is host local address.
     */
    CandidateType_Host,
    /**
     * \~English
     * The address is server reflexive address.
     */
    CandidateType_ServerReflexive,
    /**
     * \~English
     * The address is peer reflexive address.
     */
    CandidateType_PeerReflexive,
    /**
     * \~English
     * The address is relayed address.
     */
    CandidateType_Relayed,
} CandidateType;

/**
 * \~English
 * Peers network topology type.
 */
typedef enum NetworkTopology {
    /**
     * \~English
     * The stream peers is in LAN, using direct connection.
     */
    NetworkTopology_LAN,
    /**
     * \~English
     * The stream peers behind NAT, using P2P direct connection.
     */
    NetworkTopology_P2P,
    /**
     * \~English
     * The stream peers behind NAT, using relayed connection.
     */
    NetworkTopology_RELAYED,
} NetworkTopology;

/**
 * \~English
 * Carrier stream address information.
 */
typedef struct CarrierAddressInfo {
    /**
     * \~English
     * The candidate address type.
     */
    CandidateType type;
    /**
     * \~English
     * The IP/host of the address.
     */
    char addr[CARRIER_MAX_IP_STRING_LEN + 1];
    /**
     * \~English
     * The port of the address.
     */
    int port;
    /**
     * \~English
     * The IP/host of the related address.
     */
    char related_addr[CARRIER_MAX_IP_STRING_LEN + 1];
    /**
     * \~English
     * The port of the related address.
     */
    int related_port;
} CarrierAddressInfo;

/**
 * \~English
 * Carrier stream transport information.
 */
typedef struct CarrierTransportInfo {
    /**
     * \~English
     * The network topology type: LAN, P2P or relayed.
     */
    NetworkTopology topology;
    /**
     * \~English
     * The local address information.
     */
    CarrierAddressInfo local;
    /**
     * \~English
     * The remote address information.
     */
    CarrierAddressInfo remote;
} CarrierTransportInfo;

/* Global session APIs */

/**
 *\~English
 * Make initialization on Android platform.
 *
 * @return
 *      true if initialization succeeded, or false if not.
 */
#if defined(__ANDROID__)
CARRIER_API
bool carrier_session_jni_onload(void *vm, void *reserved);
#endif

/**
 * \~English
 * An application-defined function that handle session requests.
 *
 * CarrierSessionRequestCallback is the callback function type.
 *
 * @param
 *      carrier     [in] A handle to the Carrier node instance.
 * @param
 *      from        [in] The id from who send the message.
 * @param
 *      bundle      [in] The bundle of this session.
 * @param
 *      sdp         [in] The remote users SDP. End the null terminal.
 *                       Reference: https://tools.ietf.org/html/rfc4566
 * @param
 *      len         [in] The length of the SDP.
 * @param
 *      context     [in] The application defined context data.
 */
typedef void CarrierSessionRequestCallback(Carrier *carrier, const char *from,
        const char *bundle, const char *sdp, size_t len, void *context);

/**
 * \~English
 * Initialize carrier session extension.
 *
 * The application must initialize the session extension before calling
 * any session API.
 *
 * @param
 *      carrier     [in] A handle to the Carrier node instance.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling carrier_get_error().
 */
CARRIER_API
int carrier_session_init(Carrier *carrier);

/**
 * \~English
 * Clean up Carrier session extension.
 *
 * The application should call carrier_session_cleanup before quit,
 * to clean up the resources associated with the extension.
 *
 * If the extension is not initialized, this function has no effect.
 *
 * @param
 *      carrier [in] A handle to the carrier node instance.
 */
CARRIER_API
void carrier_session_cleanup(Carrier *carrier);

/**
 * \~English
 * Set session request callback.
 *
 * @param
 *      carrier     [in] A handle to the carrier node instance.
 * @param
 *      bundle_prefix
 *                  [in] The prefix of bundle.
 * @param
 *      callback
 *                  [in] The callback function to process this request.
 * @param
 *      context
 *                  [in] The application defined context data.
 *
 * @return
 *      If no error occurs, return 0.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling carrier_get_error().
 */
CARRIER_API
int carrier_session_set_callback(Carrier *carrier, const char *bundle_prefix,
        CarrierSessionRequestCallback *callback, void *context);

/**
 * \~English
 * Create a new session to a friend.
 *
 * The session object represent a conversation handle to a friend.
 *
 * @param
 *      carrier     [in] A handle to the carrier node instance.
 * @param
 *      address     [in] The target address.
 *
 * @return
 *      If no error occurs, return the pointer of CarrierSession object.
 *      Otherwise, return NULL, and a specific error code can be
 *      retrieved by calling carrier_get_error().
 */
CARRIER_API
CarrierSession *carrier_session_new(Carrier *carrier, const char *address);

/**
 * \~English
 * Close a session to friend. All resources include streams, multiplexers
 * associated with current session will be destroyed.
 *
 * @param
 *      session     [in] A handle to the carrier session.
 */
CARRIER_API
void carrier_session_close(CarrierSession *session);

/**
 * \~English
 * Get the remote peer's address of the session.
 *
 * @param
 *      session     [in] A handle to the carrier session.
 * @param
 *      address     [out] The buffer that will receive the peer address.
 *                        The buffer size should at least
 *                        (2 * ELA_MAX_ID_LEN + 1) bytes.
 * @param
 *      len         [in] The buffer size of appid.
 *
 * @return
 *      The remote peer string address, or NULL if buffer is too small.
 */
CARRIER_API
char *carrier_session_get_peer(CarrierSession *session, char *address, size_t len);

/**
 * \~English
 * Set the arbitary user data to be associated with the session.
 *
 * @param
 *      session     [in] A handle to the carrier session.
 * @param
 *      userdata    [in] Arbitary user data to be associated with this session.
 */
CARRIER_API
void carrier_session_set_userdata(CarrierSession *session, void *userdata);

/**
 * \~English
 * Get the user data associated with the session.
 *
 * @param
 *      session     [in] A handle to the carrier session.
 *
 * @return
 *      The user data associated with session.
 */
CARRIER_API
void *carrier_session_get_userdata(CarrierSession *session);

/**
 * \~English
 * An application-defined function that receive session request complete
 * event.
 *
 * CarrierSessionRequestCompleteCallback is the callback function type.
 *
 * @param
 *      session     [in] A handle to the CarrierSession.
 * @param
 *      bundle      [in] The bundle of this session.
 * @param
 *      status      [in] The status code of the response.
 *                       0 is success, otherwise is error.
 * @param
 *      reason      [in] The error message if status is error, or NULL
 * @param
 *      sdp         [in] The remote users SDP. End the null terminal.
 *                       Reference: https://tools.ietf.org/html/rfc4566
 * @param
 *      len         [in] The length of the SDP.
 * @param
 *      context     [in] The application defined context data.
 */
typedef void CarrierSessionRequestCompleteCallback(CarrierSession *session,
        const char *bundle, int status, const char *reason,
        const char *sdp, size_t len, void *context);

/**
 * \~English
 * Send session request to the friend.
 *
 * @param
 *      session     [in] A handle to the CarrierSession.
 * @param
 *      bundle      [in] The bundle of this session.
 * @param
 *      callback    [in] A pointer to CarrierSessionRequestCompleteCallback
 *                       function to receive the session response.
 * @param
 *      context      [in] The application defined context data.
 *
 * @return
 *      0 if the session request successfully send to the friend.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling carrier_get_error().
 */
CARRIER_API
int carrier_session_request(CarrierSession *session, const char *bundle,
        CarrierSessionRequestCompleteCallback *callback, void *context);

/**
 * \~English
 * Reply the session request from friend.
 *
 * This function will send a session response to friend.
 *
 * @param
 *      session     [in] A handle to the CarrierSession.
 * @param
 *      bundle      [in] The bundle of this session.
 * @param
 *      status      [in] The status code of the response.
 *                       0 is success, otherwise is error.
 * @param
 *      reason      [in] The error message if status is error, or NULL
 *                       if success.
 *
 * @return
 *      0 if the session response successfully send to the friend.
 *      Otherwise, return -1, and a specific error code can be
 *      retrieved by calling carrier_get_error().
 */
CARRIER_API
int carrier_session_reply_request(CarrierSession *session, const char *bundle,
        int status, const char* reason);

/**
 * \~English
 * Begin to start a session.
 *
 * All streams in current session will try to connect with remote friend,
 * the stream status will update to application by stream's callbacks.
 *
 * @param
 *      session     [in] A handle to the CarrierSession.
 * @param
 *      sdp         [in] The remote users SDP. End the null terminal.
 *                       Reference: https://tools.ietf.org/html/rfc4566
 * @param
 *      len         [in] The length of the SDP.
 *
 * @return
 *      0 on success, or -1 if an error occurred. The specific error code
 *      can be retrieved by calling carrier_get_error().
 */
CARRIER_API
int carrier_session_start(CarrierSession *session, const char *sdp, size_t len);

/* Session stream APIs */

/**
 * \~English
 * Carrier stream state.
 * The stream status will be changed according to the phase of the stream.
 */
typedef enum CarrierStreamState {
    /** Initialized stream */
    CarrierStreamState_initialized = 1,
    /** The underlying transport is ready for the stream. */
    CarrierStreamState_transport_ready,
    /** The stream is trying to connecting the remote. */
    CarrierStreamState_connecting,
    /** The stream connected with remote */
    CarrierStreamState_connected,
    /** The stream is deactivated */
    CarrierStreamState_deactivated,
    /** The stream closed normally */
    CarrierStreamState_closed,
    /** The stream is failed, cannot to continue. */
    CarrierStreamState_failed
} CarrierStreamState;

/**
 * \~English
 * Portforwarding supported protocols.
 */
typedef enum PortForwardingProtocol {
    /** TCP protocol. */
    PortForwardingProtocol_TCP = 1
} PortForwardingProtocol;

/**
 * \~English
 * Multiplexing channel close reason code.
 */
typedef enum CloseReason {
    /* Channel closed normally. */
    CloseReason_Normal = 0,
    /* Channel closed because timeout. */
    CloseReason_Timeout = 1,
    /* Channel closed because error ocurred. */
    CloseReason_Error = 2
} CloseReason;

/**
 * \~English
 * Carrier stream callbacks.
 *
 * Include stream status callback, stream data callback, and multiplexing
 * callbacks.
 */
typedef struct CarrierStreamCallbacks {
    /* Common callbacks */
    /**
     * \~English
     * Callback to report status of various stream operations.
     *
     * @param
     *      session     [in] The handle to the CarrierSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      state       [in] Stream state defined in CarrierStreamState.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*state_changed)(CarrierSession *session, int stream,
                          CarrierStreamState state, void *context);

    /* Stream callbacks */
    /**
     * \~English
     * Callback will be called when the stream receives
     * incoming packet.
     *
     * If the stream enabled multiplexing mode, application will not
     * receive stream_data callback any more. All data will reported
     * as multiplexing channel data.
     *
     * @param
     *      session     [in] The handle to the CarrierSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      data        [in] The received packet data.
     * @param
     *      len         [in] The received data length.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*stream_data)(CarrierSession *session, int stream,
                        const void *data, size_t len, void *context);

    /* Multiplexer callbacks */
    /**
     * \~English
     * Callback will be called when new multiplexing channel request to open.
     *
     * @param
     *      session     [in] The handle to the CarrierSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      cookie      [in] Application defined string data receive from peer.
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      True on success, or false if an error occurred.
     *      The channel will continue to open only this callback return true,
     *      otherwise the channel will be closed.
     */
    bool (*channel_open)(CarrierSession *session, int stream, int channel,
                         const char *cookie, void *context);

    /**
     * \~English
     * Callback will be called when new multiplexing channel opened.
     *
     * @param
     *      session     [in] The handle to the CarrierSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*channel_opened)(CarrierSession *session, int stream, int channel,
                           void *context);

    /**
     * \~English
     * Callback will be called when channel close.
     *
     * @param
     *      session     [in] The handle to the CarrierSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      reason      [in] Channel close reason code, defined in CloseReason.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*channel_close)(CarrierSession *session, int stream, int channel,
                          CloseReason reason, void *context);

    /**
     * \~English
     * Callback will be called when channel received incoming data.
     *
     * @param
     *      session     [in] The handle to the CarrierSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      data        [in] The received data(NULL if @len is zero).
     * @param
     *      len         [in] The received data length.
     * @param
     *      context     [in] The application defined context data.
     *
     * @return
     *      True on success, or false if an error occurred.
     *      If this callback return false, the channel will be closed
     *      with CloseReason_Error.
     */
    bool (*channel_data)(CarrierSession *session, int stream, int channel,
                         const void *data, size_t len, void *context);

    /**
     * \~English
     * Callback will be called when remote peer ask to pend data sending.
     *
     * @param
     *      session     [in] The handle to the CarrierSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*channel_pending)(CarrierSession *session, int stream, int channel,
                            void *context);

    /**
     * \~English
     * Callback will be called when remote peer ask to resume data sending.
     *
     * @param
     *      session     [in] The handle to the CarrierSession.
     * @param
     *      stream      [in] The stream ID.
     * @param
     *      channel     [in] The current channel ID.
     * @param
     *      context     [in] The application defined context data.
     */
    void (*channel_resume)(CarrierSession *session, int stream, int channel,
                           void *context);
} CarrierStreamCallbacks;

/**
 * Compress option, indicates data would be compressed before transmission.
 * For now, just reserved this bit option for future implement.
 */
#define CARRIER_STREAM_COMPRESS             0x01

/**
 * Encrypt option, indicates data would be transmitted with plain mode.
 * which means that transmitting data would be encrypted in default.
 */
#define CARRIER_STREAM_PLAIN                0x02

/**
 * Relaible option, indicates data transmission would be reliable, and be
 * guranteed to received by remote peer, which acts as TCP transmission
 * protocol. Without this option bitwised, the transmission would be
 * unreliable as UDP transmission protocol.
 */
#define CARRIER_STREAM_RELIABLE             0x04

/**
 * Multiplexing option, indicates multiplexing would be activated on
 * enstablished stream, and need to use multipexing APIs related with channel
 * instread of APIs related strema to send/receive data.
 */
#define CARRIER_STREAM_MULTIPLEXING         0x08

/**
 * PortForwarding option, indicates port forwarding would be activated
 * on established stream. This options should bitwise with 'Multiplexing'
 * option.
 */
#define CARRIER_STREAM_PORT_FORWARDING      0x10

/**
 * \~English
 * Add a new stream to session.
 *
 * Carrier stream supports several underlying transport mechanisms:
 *
 *   - Plain/encrypted UDP data gram protocol
 *   - Plain/encrypted TCP like reliable stream protocol
 *   - Multiplexing over UDP
 *   - Multiplexing over TCP like reliable protocol
 *
 *  Application can use options to specify the new stream mode. Data
 *  transferred on stream is defaultly encrypted.  Multiplexing over UDP can
 *  not provide reliable transport.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      type        [in] The stream type defined in CarrierStreamType.
 * @param
 *      options     [in] The stream mode options. options are constructed
 *                       by a bitwise-inclusive OR of flags from the
 *                       following list:
 *
 *                       - CARRIER_STREAM_PLAIN
 *                         Plain mode.
 *                       - CARRIER_STREAM_RELIABLE
 *                         Reliable mode.
 *                       - CARRIER_STREAM_MULTIPLEXING
 *                         Multiplexing mode.
 *                       - CARRIER_STREAM_PORT_FORWARDING
 *                         Support portforwarding over multiplexing.
 *
 * @param
 *      callbacks   [in] The Application defined callback functions in
 *                       CarrierStreamCallbacks.
 * @param
 *      context     [in] The application defined context data.
 *
 * @return
 *      Return stream id on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_session_add_stream(CarrierSession *session, CarrierStreamType type,
                 int options, CarrierStreamCallbacks *callbacks, void *context);

/**
 * \~English
 * Remove a stream from session.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream id to be removed.
 *
 * @return
 *      0 on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_session_remove_stream(CarrierSession *session, int stream);

/**
 * \~English
 * Add a new portforwarding service to session.
 *
 * The registered services can be used by remote peer in portforwarding
 * request.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      service     [in] The new service name, should be unique
 *                       in session scope.
 * @param
 *      protocol    [in] The protocol of the service.
 * @param
 *      host        [in] The host name or ip of the service.
 * @param
 *      port        [in] The port of the service.
 *
 * @return
 *      0 on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_session_add_service(CarrierSession *session, const char *service,
        PortForwardingProtocol protocol, const char *host, const char *port);

/**
 * \~English
 * Remove a portforwarding service to session.
 *
 * This function has not effect on existing portforwarings.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      service     [in] The service name.
 */
CARRIER_API
void carrier_session_remove_service(CarrierSession *session, const char *service);

/**
 * \~English
 * Get the carrier stream type.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      type        [out] The stream type defined in CarrierStreamType.
 *
 * @return
 *      0 on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_stream_get_type(CarrierSession *session, int stream,
                        CarrierStreamType *type);

/**
 * \~English
 * Get the carrier stream current state.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      state       [out] The stream state defined in CarrierStreamState.
 *
 * @return
 *      0 on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_stream_get_state(CarrierSession *session, int stream,
                         CarrierStreamState *state);

/**
 * \~English
 * Get the carrier stream transport information.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      info        [out] The stream transport information defined in
 *                        CarrierTransportInfo.
 *
 * @return
 *      0 on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_stream_get_transport_info(CarrierSession *session, int stream,
                                  CarrierTransportInfo *info);

/**
 * \~English
 * Send outgoing data to remote peer.
 *
 * If the stream is in multiplexing mode, application can not
 * call this function to send data. If this function is called
 * on multiplexing mode stream, it will return error.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      data        [in] The outgoing data.
 * @param
 *      len         [in] The outgoing data length.
 *
 * @return
 *      Sent bytes on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
ssize_t carrier_stream_write(CarrierSession *session, int stream,
                         const void *data, size_t len);

/**
 * \~English
 * Open a new channel on multiplexing stream.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      cookie      [in] Application defined data pass to remote peer.
 *
 * @return
 *      New channel ID on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_stream_open_channel(CarrierSession *session, int stream,
                            const char *cookie);

/**
 * \~English
 * Close a new channel on multiplexing stream.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      channel     [in] The channel ID.
 *
 * @return
 *      0 on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_stream_close_channel(CarrierSession *session, int stream, int channel);

/**
 * \~English
 * Send outgoing data to remote peer.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      channel     [in] The channel ID.
 * @param
 *      data        [in] The outgoing data(MUST be NULL if @len is zero).
 * @param
 *      len         [in] The outgoing data length(COULD be zero, the receiver
 *                       will get CarrierStreamCallbacks::channel_data callback with
 *                       argument @len being zero).
 *
 * @return
 *      Sent bytes on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
ssize_t carrier_stream_write_channel(CarrierSession *session, int stream,
                    int channel, const void *data, size_t len);

/**
 * \~English
 * Request remote peer to pend channel data sending.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      channel     [in] The channel ID.
 *
 * @return
 *      0 on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_stream_pend_channel(CarrierSession *session, int stream, int channel);

/**
 * \~English
 * Request remote peer to resume channel data sending.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      channel     [in] The channel ID.
 *
 * @return
 *      0 on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_stream_resume_channel(CarrierSession *session, int stream, int channel);

/**
 * \~English
 * Open a portforwarding to remote service over multiplexing.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      service     [in] The remote service name.
 * @param
 *      protocol    [in] Portforwarding protocol.
 * @param
 *      host        [in] Local host or ip to binding.
 *                       If host is NULL, portforwarding will bind to 127.0.0.1.
 * @param
 *      port        [in] Local port to binding, can not be NULL.
 *
 * @return
 *      Portforwarding ID on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_stream_open_port_forwarding(CarrierSession *session, int stream,
        const char *service, PortForwardingProtocol protocol,
        const char *host, const char *port);

/**
 * \~English
 * Close a portforwarding to remote service over multiplexing.
 *
 * If the stream is not multiplexing this function will fail.
 *
 * @param
 *      session     [in] The handle to the CarrierSession.
 * @param
 *      stream      [in] The stream ID.
 * @param
 *      portforwarding  [in] The portforwarding ID.
 *
 * @return
 *      0 on success, or -1 if an error occurred.
 *      The specific error code can be retrieved by calling
 *      carrier_get_error().
 */
CARRIER_API
int carrier_stream_close_port_forwarding(CarrierSession *session, int stream,
                                     int portforwarding);

#ifdef __cplusplus
}
#endif

#if defined(__APPLE__)
#pragma GCC diagnostic pop
#endif

#include <carrier_session_deprecated.h>

#endif /* __ELASTOS_CARRIER_SESSION_H__ */
