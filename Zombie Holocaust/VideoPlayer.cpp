// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include "VideoPlayer.h"
#include <assert.h>

#pragma comment(lib, "shlwapi")

template <class Q>
HRESULT GetEventObject(IMFMediaEvent *pEvent, Q **ppObject)
{
    *ppObject = NULL;   // zero output

    PROPVARIANT var;	HRESULT hr = pEvent->GetValue(&var);
    
	if (SUCCEEDED(hr))
    {
		if (var.vt == VT_UNKNOWN) { hr = var.punkVal->QueryInterface(ppObject); }
		else { hr = MF_E_INVALIDTYPE; }
        
		PropVariantClear(&var);
    }
    return hr;
}

HRESULT CreateMediaSource(PCWSTR pszURL, IMFMediaSource **ppSource);

HRESULT CreatePlaybackTopology(IMFMediaSource *pSource, IMFPresentationDescriptor *pPD, HWND hVideoWnd, IMFTopology **ppTopology);

//  Static class method to create the VideoPlayer object.

HRESULT VideoPlayer::CreateInstance(HWND hVideo, HWND hEvent, VideoPlayer **ppPlayer)
{
	if (ppPlayer == NULL) { return E_POINTER; }

    VideoPlayer *pPlayer = new (std::nothrow) VideoPlayer(hVideo, hEvent);
	if (pPlayer == NULL) { return E_OUTOFMEMORY; }

    HRESULT hr = pPlayer->Initialize();
	if (SUCCEEDED(hr)) { *ppPlayer = pPlayer; }
	else { pPlayer->Release(); }
    
	return hr;
}

HRESULT VideoPlayer::Initialize()
{
    // Start up Media Foundation platform.
    HRESULT hr = MFStartup(MF_VERSION);

    if (SUCCEEDED(hr))
    {
        m_hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hCloseEvent == NULL) { hr = HRESULT_FROM_WIN32(GetLastError()); }
    }
    return hr;
}

VideoPlayer::VideoPlayer(HWND hVideo, HWND hEvent) :	m_pSession(NULL),		m_pSource(NULL),	m_pVideoDisplay(NULL),	m_hwndVideo(hVideo),
												m_hwndEvent(hEvent),	m_state(Closed),	m_hCloseEvent(NULL),	m_nRefCount(1)	{}

VideoPlayer::~VideoPlayer()
{
    assert(m_pSession == NULL);

    Shutdown();
}

HRESULT VideoPlayer::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] = { QITABENT(VideoPlayer, IMFAsyncCallback),{ 0 } };
    return QISearch(this, qit, riid, ppv);
}

ULONG VideoPlayer::AddRef()
{
    return InterlockedIncrement(&m_nRefCount);
}

ULONG VideoPlayer::Release()
{
    ULONG uCount = InterlockedDecrement(&m_nRefCount);
	if (uCount == 0) { delete this; }

    return uCount;
}

//  Open a URL for playback.
HRESULT VideoPlayer::OpenURL(const WCHAR *sURL)
{
    IMFTopology *pTopology = NULL;
    IMFPresentationDescriptor* pSourcePD = NULL;

    // Create the media session.
    HRESULT hr = CreateSession();
	if (FAILED(hr)) { goto done; };

    // Create the media source.
    hr = CreateMediaSource(sURL, &m_pSource);
	if (FAILED(hr)) { goto done; };

    // Create the presentation descriptor for the media source.
    hr = m_pSource->CreatePresentationDescriptor(&pSourcePD);
	if (FAILED(hr)) { goto done; }

    // Create a partial topology.
    hr = CreatePlaybackTopology(m_pSource, pSourcePD, m_hwndVideo, &pTopology);
	if (FAILED(hr)) { goto done; }

    // Set the topology on the media session.
    hr = m_pSession->SetTopology(0, pTopology);
	if (FAILED(hr)) { goto done; }

    m_state = OpenPending;

done:
	//MESessionTopologySet event.
	if (FAILED(hr)) { m_state = Closed; }

    SafeRelease(&pSourcePD);
    SafeRelease(&pTopology);
    return hr;
}

//  Pause playback.
HRESULT VideoPlayer::Pause()    
{
	if (m_state != Started) { return MF_E_INVALIDREQUEST; }
    
	if (m_pSession == NULL || m_pSource == NULL) { return E_UNEXPECTED; }

    HRESULT hr = m_pSession->Pause();
	if (SUCCEEDED(hr)) { m_state = Paused; }

    return hr;
}

// Stop playback.
HRESULT VideoPlayer::Stop()
{
	if (m_state != Started && m_state != Paused) { return MF_E_INVALIDREQUEST; }

	if (m_pSession == NULL) { return E_UNEXPECTED; }

    HRESULT hr = m_pSession->Stop();
	if (SUCCEEDED(hr)) { m_state = Stopped; }

    return hr;
}

//  Repaint the video window. Call this method on WM_PAINT.

HRESULT VideoPlayer::Repaint()
{
	if (m_pVideoDisplay) { return m_pVideoDisplay->RepaintVideo(); }
    
	else { return S_OK; }
}

//  Resize the video rectangle.
//  Call this method if the size of the video window changes
HRESULT VideoPlayer::ResizeVideo(WORD width, WORD height)
{
    if (m_pVideoDisplay)
	{
		RECT rcDest = { 0, 0, width, height };

        return m_pVideoDisplay->SetVideoPosition(NULL, &rcDest);
    }
	else { return S_OK; }
}

//  Callback for the asynchronous BeginGetEvent method.
HRESULT VideoPlayer::Invoke(IMFAsyncResult *pResult)
{
    MediaEventType meType = MEUnknown;  // Event type

    IMFMediaEvent *pEvent = NULL;

    // Get the event from the event queue.
    HRESULT hr = m_pSession->EndGetEvent(pResult, &pEvent);
	if (FAILED(hr)) { goto done; }

    // Get the event type. 
    hr = pEvent->GetType(&meType);
	if (FAILED(hr)) { goto done; }

    if (meType == MESessionClosed)
    {
        // The session was closed. 
        SetEvent(m_hCloseEvent);
    }
    else
    {
        // For all other events, get the next event in the queue.
        hr = m_pSession->BeginGetEvent(this, NULL);
		if (FAILED(hr)) { goto done; }
    }

    // Check the application state. 
    if (m_state != Closing)
    {
        // Leave a reference count on the event.
        pEvent->AddRef();

		PostMessage(m_hwndEvent, WM_APP_PLAYER_EVENT, (WPARAM)pEvent, (LPARAM)meType);
    }

done:
    SafeRelease(&pEvent);
    return S_OK;
}

HRESULT VideoPlayer::HandleEvent(UINT_PTR pEventPtr)
{
    HRESULT hrStatus = S_OK;            
    MediaEventType meType = MEUnknown;  

    IMFMediaEvent *pEvent = (IMFMediaEvent*)pEventPtr;

	if (pEvent == NULL) { return E_POINTER; }

    // Get the event type.
    HRESULT hr = pEvent->GetType(&meType);
	if (FAILED(hr)) { goto done; }

    // Get the event status. If the operation that triggered the event 
    // did not succeed, the status is a failure code.
    hr = pEvent->GetStatus(&hrStatus);

    // Check if the async operation succeeded.
	if (SUCCEEDED(hr) && FAILED(hrStatus)) { hr = hrStatus; }
	
	if (FAILED(hr)) { goto done; }

    switch(meType)
    {
    case MESessionTopologyStatus:
        hr = OnTopologyStatus(pEvent);
        break;

    case MEEndOfPresentation:
        hr = OnPresentationEnded(pEvent);
        break;

    case MENewPresentation:
        hr = OnNewPresentation(pEvent);
        break;

    default:
        hr = OnSessionEvent(pEvent, meType);
        break;
    }

done:
    SafeRelease(&pEvent);
    return hr;
}

//  Release all resources held by this object.
HRESULT VideoPlayer::Shutdown()
{
    // Close the session
    HRESULT hr = CloseSession();

    // Shutdown the Media Foundation platform
    MFShutdown();

    if (m_hCloseEvent)
    {
        CloseHandle(m_hCloseEvent);
        m_hCloseEvent = NULL;
    }
    return hr;
}

// Protected methods

HRESULT VideoPlayer::OnTopologyStatus(IMFMediaEvent *pEvent)
{
    UINT32 status; 

    HRESULT hr = pEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &status);
    if (SUCCEEDED(hr) && (status == MF_TOPOSTATUS_READY))
    {
        SafeRelease(&m_pVideoDisplay);

        // Get the IMFVideoDisplayControl interface from EVR. This call is
        // expected to fail if the media file does not have a video stream.

		(void)MFGetService(m_pSession, MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&m_pVideoDisplay));

        hr = StartPlayback();
    }
    return hr;
}


//  Handler for MEEndOfPresentation event.
HRESULT VideoPlayer::OnPresentationEnded(IMFMediaEvent *pEvent)
{
    // The session puts itself into the stopped state automatically.
    m_state = Stopped;
    return S_OK;
}

//  Handler for MENewPresentation event. 

HRESULT VideoPlayer::OnNewPresentation(IMFMediaEvent *pEvent)
{
    IMFPresentationDescriptor *pPD = NULL;
    IMFTopology *pTopology = NULL;

    // Get the presentation descriptor from the event.
    HRESULT hr = GetEventObject(pEvent, &pPD);
	if (FAILED(hr)) { goto done; }

    // Create a partial topology.
    hr = CreatePlaybackTopology(m_pSource, pPD,  m_hwndVideo,&pTopology);
	if (FAILED(hr)) { goto done; }

    // Set the topology on the media session.
    hr = m_pSession->SetTopology(0, pTopology);
	if (FAILED(hr)) { goto done; }

    m_state = OpenPending;

done:
    SafeRelease(&pTopology);
    SafeRelease(&pPD);
    return S_OK;
}

//  Create a new instance of the media session.
HRESULT VideoPlayer::CreateSession()
{
    // Close the old session, if any.
    HRESULT hr = CloseSession();
	if (FAILED(hr)) { goto done; }

    assert(m_state == Closed);

    // Create the media session.
    hr = MFCreateMediaSession(NULL, &m_pSession);
	if (FAILED(hr)) { goto done; }

    // Start pulling events from the media session
    hr = m_pSession->BeginGetEvent((IMFAsyncCallback*)this, NULL);
	if (FAILED(hr)) { goto done; }

    m_state = Ready;

done:
    return hr;
}

//  Close the media session. 
HRESULT VideoPlayer::CloseSession()
{
    //  The IMFMediaSession::Close method is asynchronous, but the 
    //  VideoPlayer::CloseSession method waits on the MESessionClosed event.
    //  
    //  MESessionClosed is guaranteed to be the last event that the 
    //  media session fires.

    HRESULT hr = S_OK;

    SafeRelease(&m_pVideoDisplay);

    // First close the media session.
    if (m_pSession)
    {
        DWORD dwWaitResult = 0;

        m_state = Closing;
           
        hr = m_pSession->Close();
        // Wait for the close operation to complete
        if (SUCCEEDED(hr))
        {
            dwWaitResult = WaitForSingleObject(m_hCloseEvent, 5000);
			if (dwWaitResult == WAIT_TIMEOUT) { assert(FALSE); }
            // Now there will be no more events from this session.
        }
    }

    // Complete shutdown operations.
    if (SUCCEEDED(hr))
    {
        // Shut down the media source. (Synchronous operation, no events.)
		if (m_pSource) { (void)m_pSource->Shutdown(); }
        
        // Shut down the media session. (Synchronous operation, no events.)
		if (m_pSession) { (void)m_pSession->Shutdown(); }
    }
    SafeRelease(&m_pSource);
    SafeRelease(&m_pSession);
    m_state = Closed;
    return hr;
}

//  Start playback from the current position. 
HRESULT VideoPlayer::StartPlayback()
{
    assert(m_pSession != NULL);

    PROPVARIANT varStart;
    PropVariantInit(&varStart);

    HRESULT hr = m_pSession->Start(&GUID_NULL, &varStart);
	if (SUCCEEDED(hr)) { m_state = Started; }
    
	PropVariantClear(&varStart);
    return hr;
}

//  Start playback from paused or stopped.
HRESULT VideoPlayer::Play()
{
	if (m_state != Paused && m_state != Stopped) { return MF_E_INVALIDREQUEST; }

	if (m_pSession == NULL || m_pSource == NULL) { return E_UNEXPECTED; }
    return StartPlayback();
}


//  Create a media source from a URL.
HRESULT CreateMediaSource(PCWSTR sURL, IMFMediaSource **ppSource)
{
    MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;

    IMFSourceResolver* pSourceResolver = NULL;
    IUnknown* pSource = NULL;

    // Create the source resolver.
    HRESULT hr = MFCreateSourceResolver(&pSourceResolver);
	if (FAILED(hr)) { goto done; }

    // Use the source resolver to create the media source.
	hr = pSourceResolver->CreateObjectFromURL(sURL, MF_RESOLUTION_MEDIASOURCE, NULL, &ObjectType, &pSource);
	if (FAILED(hr)) { goto done; }

    // Get the IMFMediaSource interface from the media source.
    hr = pSource->QueryInterface(IID_PPV_ARGS(ppSource));

done:
    SafeRelease(&pSourceResolver);
    SafeRelease(&pSource);
    return hr;
}

//  Create an activation object for a renderer, based on the stream media type.

HRESULT CreateMediaSinkActivate(IMFStreamDescriptor *pSourceSD, HWND hVideoWindow, IMFActivate **ppActivate)
{
    IMFMediaTypeHandler *pHandler = NULL;
    IMFActivate *pActivate = NULL;

    // Get the media type handler for the stream.
    HRESULT hr = pSourceSD->GetMediaTypeHandler(&pHandler);
	if (FAILED(hr)) { goto done; }

    // Get the major media type.
    GUID guidMajorType;
    hr = pHandler->GetMajorType(&guidMajorType);
	if (FAILED(hr)) { goto done; }
 
    // Create an IMFActivate object for the renderer, based on the media type.
    if (MFMediaType_Audio == guidMajorType)
    {
        // Create the audio renderer.
        hr = MFCreateAudioRendererActivate(&pActivate);
    }
    else if (MFMediaType_Video == guidMajorType)
    {
        // Create the video renderer.
        hr = MFCreateVideoRendererActivate(hVideoWindow, &pActivate);
    }
	else { hr = E_FAIL; }
	if (FAILED(hr)) { goto done; };
 
    // Return IMFActivate pointer to caller.
    *ppActivate = pActivate;
    (*ppActivate)->AddRef();

done:
    SafeRelease(&pHandler);
    SafeRelease(&pActivate);
    return hr;
}

// Add a source node to a topology.
HRESULT AddSourceNode(IMFTopology *pTopology, IMFMediaSource *pSource, IMFPresentationDescriptor *pPD, IMFStreamDescriptor *pSD, IMFTopologyNode **ppNode)
{
    IMFTopologyNode *pNode = NULL;

    // Create the node.
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode);
	if (FAILED(hr)) { goto done; }

    // Set the attributes.
    hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pSource);
	if (FAILED(hr)) { goto done; }

    hr = pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD);
	if (FAILED(hr)) { goto done; }

    hr = pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);
	if (FAILED(hr)) { goto done; }
    
    // Add the node to the topology.
    hr = pTopology->AddNode(pNode);
	if (FAILED(hr)) { goto done; }

    // Return the pointer to the caller.
    *ppNode = pNode;
    (*ppNode)->AddRef();

done:
    SafeRelease(&pNode);
    return hr;
}

// Add an output node to a topology.
HRESULT AddOutputNode(IMFTopology *pTopology, IMFActivate *pActivate, DWORD dwId, IMFTopologyNode **ppNode)
{
    IMFTopologyNode *pNode = NULL;

    // Create the node.
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode);
	if (FAILED(hr)) { goto done; }

    // Set the object pointer.
    hr = pNode->SetObject(pActivate);
	if (FAILED(hr)) { goto done; }

    // Set the stream sink ID attribute.
    hr = pNode->SetUINT32(MF_TOPONODE_STREAMID, dwId);
	if (FAILED(hr)) { goto done; }

    hr = pNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
	if (FAILED(hr)) { goto done; }

    // Add the node to the topology.
    hr = pTopology->AddNode(pNode);
	if (FAILED(hr)) { goto done; }

    // Return the pointer to the caller.
    *ppNode = pNode;
    (*ppNode)->AddRef();

done:
    SafeRelease(&pNode);
    return hr;
}

HRESULT AddBranchToPartialTopology(IMFTopology *pTopology, IMFMediaSource *pSource, IMFPresentationDescriptor *pPD, DWORD iStream, HWND hVideoWnd)
{
    IMFStreamDescriptor *pSD = NULL;			IMFActivate         *pSinkActivate = NULL;
	IMFTopologyNode     *pSourceNode = NULL;	IMFTopologyNode     *pOutputNode = NULL;

    BOOL fSelected = FALSE;

    HRESULT hr = pPD->GetStreamDescriptorByIndex(iStream, &fSelected, &pSD);
	if (FAILED(hr)) { goto done; }

    if (fSelected)
    {
        // Create the media sink activation object.
        hr = CreateMediaSinkActivate(pSD, hVideoWnd, &pSinkActivate);
		if (FAILED(hr)) { goto done; }

        // Add a source node for this stream.
        hr = AddSourceNode(pTopology, pSource, pPD, pSD, &pSourceNode);
		if (FAILED(hr)) { goto done; }

        // Create the output node for the renderer.
        hr = AddOutputNode(pTopology, pSinkActivate, 0, &pOutputNode);
		if (FAILED(hr)) { goto done; }

        // Connect the source node to the output node.
        hr = pSourceNode->ConnectOutput(0, pOutputNode, 0);
    }
    // else: If not selected, don't add the branch. 

done:
    SafeRelease(&pSD);
    SafeRelease(&pSinkActivate);
    SafeRelease(&pSourceNode);
    SafeRelease(&pOutputNode);
    return hr;
}

//  Create a playback topology from a media source.
HRESULT CreatePlaybackTopology(IMFMediaSource *pSource, IMFPresentationDescriptor *pPD, HWND hVideoWnd, IMFTopology **ppTopology)
{
    IMFTopology *pTopology = NULL;
    DWORD cSourceStreams = 0;

    // Create a new topology.
    HRESULT hr = MFCreateTopology(&pTopology);
    if (FAILED(hr)) { goto done; }

    // Get the number of streams in the media source.
    hr = pPD->GetStreamDescriptorCount(&cSourceStreams);
	if (FAILED(hr)) { goto done; }

    // For each stream, create the topology nodes and add them to the topology.
    for (DWORD i = 0; i < cSourceStreams; i++)
    {
        hr = AddBranchToPartialTopology(pTopology, pSource, pPD, i, hVideoWnd);
		if (FAILED(hr)) { goto done; }
    }

    // Return the IMFTopology pointer to the caller.
    *ppTopology = pTopology;
    (*ppTopology)->AddRef();

done:
    SafeRelease(&pTopology);
    return hr;
}
