<?xml version="1.0"?><doc>
<members>
<member name="T:ExitGames.Common.MemoryManagement.AllocatorInterface" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocatorinterface.h" line="17">
Custom Allocators to be used with %Photons Memory Management need to inherit and implement this interface.
The allocator that is used by %Photon can be set through setAllocator(). 
</member>
<member name="M:ExitGames.Common.MemoryManagement.AllocatorInterface.Dispose" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocatorinterface.h" line="23">
Destructor. 
</member>
<member name="M:ExitGames.Common.MemoryManagement.AllocatorInterface.setMaxAllocSize(System.UInt32)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocatorinterface.h" line="27">
This function gets called by MemoryManagement::setMaxAllocSize() and an implementation is required to behave as explained in the documentation of that function. 
</member>
<member name="M:ExitGames.Common.MemoryManagement.AllocatorInterface.alloc(System.UInt32)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocatorinterface.h" line="31">
This function gets called by #EG_MALLOC and an implementation is required to behave as explained in the documentation of that macro. 
</member>
<member name="M:ExitGames.Common.MemoryManagement.AllocatorInterface.dealloc(System.Void*)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocatorinterface.h" line="35">
This function gets called by #EG_FREE and an implementation is required to behave as explained in the documentation of that macro. 
</member>
<member name="M:ExitGames.Common.MemoryManagement.AllocatorInterface.resize(System.Void*,System.UInt32)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocatorinterface.h" line="39">
This function gets called by #EG_REALLOC and an implementation is required to behave as explained in the documentation of that macro. 
</member>
<!-- メンバー 'M:ExitGames.Common.MemoryManagement.AllocatorInterface.get' に対して形式が正しくない XML ドキュメント コメントを破棄します。-->
<member name="M:ExitGames.Common.MemoryManagement.setMaxAllocSize(System.UInt32)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocate.h" line="96">
@name HighLevelMemoryManagement
			   @anchor HighLevelMemoryManagement 
			   The template functions in this section are an alternative for the C++ dynamic memory management operators new, new[],
			   delete and delete[].

			   They are implemented in terms of enhancing the @ref LowLevelMemoryManagement "Low Level Memory Management macros" and
			   for this reason offer similar advantages over new and co like those macros offer over malloc and co.

			   However same as new and co they also construct and destruct the objects that they allocate and deallocate. 
			   Sets the max size of memory that might get allocated ahead of time as a result of a single memory request.
			   @details
			   Requesting memory from the OS is an expensive operation. This is why a good memory manager might choose to request bigger amounts of memory at once and give out smaller chunks of them to the application code. This way it can reduce the amount of
			   necessary memory requests to the OS. Depending on the memory requests that come in from the application code, a memory manager might decide to scale up its own requests to the OS.

			   You can set an upper limit for how much the currently active memory manager is allowed to scale up through this function.

			   Example:
			   Consider a pool-based memory manager that uses multiple memory pools, where each serves requests for memory of different sizes. There could be a pool for tiny memory requests, one for small requests, one for medium requests, and so on.

			   Now let's imagine that there is a pool that serves requests between 65 and 128 bytes in size each and for this purpose keeps a bunch of 128 byte blocks around to give out to requesters. In the beginning it might just keep very few such blocks around, as
			   the memory manager does not know, how many blocks of this size an app might need to use in parallel. When an app requests lots of those blocks, the pool would scale accordingly and to not need to do a request to the OS too often, it might increase the
			   size of it's own requests. i.e. at first it could have just 4 blocks around, then when it resizes, it would allocate memory for another 4 blocks, then for 8 more, then for another, 16, then 32, 64, 128, 256, 512, 1024 blocks more, and so on.

			   Now if you set an upper limit of 8192 bytes, then the pool would not increase the size of its requests to the OS beyond that limit. For that 128 byte blocks pool that would mean that it would request at max 8192/128==64 blocks at once. So the resize
			   pattern from above would change to 4, 8, 16, 32, 64, 64, 64, 64, and so on.

			   Accordingly with the same 8192 bytes limit in place a pool that holds 1024 byte blocks would not allocate memory for more than 8 such blocks at once.
			   @note
			   This does not set a limit to the overall memory that might get allocated, but only to the memory that gets allocated as a direct result of a single memory request. The very next request might already lead to another allocation if the memory manager
			   decides so (for example a pool based memory manager might serve differently sized requests from different pools that resize independently from each other).
			   @remarks
			   This function forwards the passed in value to the currently set allocator (see setAllocator() ) and does not store it itself. For this reason a call to this function only affects the settings of the currently set allocator and not those of any future
			   allocator, that might be set by setAllocator() at any point in time after this function got called.
			   @remarks
			   It is the responsibility of the allocator to honor the the setting that the user has applied through this function.
			   @param maxAllocSize the max size for a single memory request to the OS
			   @sa
			   setAllocator(), AllocatorInterface 
</member>
<member name="M:ExitGames.Common.MemoryManagement.setMaxSizeForAllocatorUsage(System.UInt32)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocate.h" line="142">
			   Sets a limit up to which memory requests get forwarded to the set allocator. Requests with a size above the limit get redirected to the OS instead.
			   @details
			   Requesting memory from the OS is an expensive operation.

			   For frequent requests of small amounts of memory it is usually more efficient to request that memory from a memory manager instead, which requests bigger amounts of memory from the OS at once, splits them up into smaller blocks and returns those
			   smaller blocks to the requester.

			   However this is effectively a trade of reduced execution time bought with increased memory usage, which is usually a good deal for frequent small requests, but a bad deal for infrequent requests of bigger amounts of memory.

			   For this reason from a certain request size on requests get forwarded directly to the OS instead of to the set allocator.

			   This function lets you set the upper limit up to which the set allocator is used.

			   Requests above the limit will be forwarded directly to the OS.
			   @remarks
			   The value that is set through this function affects all allocators, not just the currently set one.
			   @param maxSizeForAllocatorUsage the max size for a memory request up to which the set allocator gets used
			   @sa
			   setAllocator(), AllocatorInterface 
</member>
<member name="M:ExitGames.Common.MemoryManagement.setAllocator(ExitGames.Common.MemoryManagement.AllocatorInterface*!System.Runtime.CompilerServices.IsImplicitlyDereferenced)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocate.h" line="167">
			   Sets the allocator that will be used by future memory requests to the provided value.
			   @details
			   All dynamic memory allocation requests by the %Photon Client libraries go either through one of functions in MemoryManagement or through one of the @ref LowLevelMemoryManagement "Low Level Memory Management macros".
			   The application code can also use these functions and macros for its own memory requests if its developer chooses so.

			   Each request for an amount of memory that does not exceed the limit set by setMaxSizeForAllocatorUsage() gets forwarded to an allocator. %Photon provides a default general-purpose allocator that uses pool based memory management and that works well
			   for most applications.

			   However you can set your own allocator through this function and %Photon will use that allocator for any memory requests that happen afterwards.

			   Regarding potential reasons for writing your own custom allocator please see https://en.wikipedia.org/wiki/Allocator_(C%2B%2B)#Custom_allocators.
			   @remarks
			   Photons memory management stores the address of the allocator that served a specific memory request and forwards a request to free memory to the same allocator that allocated that memory.
			   @remarks
			   This means a) that you can set a different allocator as often as you like at any point in time you like and b) that you need to keep any once set allocator available even when it is no longer set as the currently used allocator, at least until you can
			   guarantee that all memory that once got requested from it, got returned to it and non of it is still in use.
			   @remarks
			   If you want to already set an initial custom allocator before any global or file-level static instances of %Photon classes get constructed, then you need to replace AllocatorInterface::get().
			   @param allocator an instance of a subclass of AllocatorInterface
			   @sa
			   setMaxSizeForAllocatorUsage(), AllocatorInterface, AllocatorInterface::get() 
</member>
<member name="M:ExitGames.Common.MemoryManagement.setAllocatorToDefault" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\common-cpp\inc\memorymanagement\allocate.h" line="194">
Calls setAllocator() with Photons default allocator as parameter.
@sa
setAllocator() 
</member>
<member name="M:ExitGames.Photon.PhotonListener.Dispose" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\photon-cpp\inc\photonlistener.h" line="29">
Destructor. 
</member>
<member name="M:ExitGames.Photon.PhotonListener.onOperationResponse(ExitGames.Photon.OperationResponse!System.Runtime.CompilerServices.IsConst*!System.Runtime.CompilerServices.IsImplicitlyDereferenced)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\photon-cpp\inc\photonlistener.h" line="33">
			 This function gets called by the library as callback to operations in response to operations sent to the Photon Server providing the response values from the server.
			 @details
			 This callback is used as general callback for all operations. The type of an operation is identified by an operation code.
		 
			 An operation's response is summarized by the return code: an int typed code, 0 for OK or some error code defined by the application, which is defining the operation itself. The opCode
			 defines the type of operation called on Photon and in turn also the return values. They are provided as a Hashtable which contains the complete reponse of Photon, including keys for
			 operation code and return code. Each operation returns its opCode and returnCode but anything else can be defined serverside.
		 
			 @param operationResponse the @link OperationResponse\endlink                      
</member>
<member name="M:ExitGames.Photon.PhotonListener.onStatusChanged(System.Int32)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\photon-cpp\inc\photonlistener.h" line="45">
			 onStatusChanged is used to denote errors or simply state-changes of the respective PhotonPeer.
			 @details
			 State change callback
		 
			 When this function is used to signalize a state-change, the statusCode will be one of these:
			 * StatusCode::CONNECT the connection to the Photon Server was established
			 * StatusCode::DISCONNECT the connection was closed (due to an API-call or a timeout)
		 
			 Furthermore this function will be called by Photon to inform about connection errors and warnings. Check StatusCode.h for a list.
		 
			 @param statusCode see description                        
</member>
<member name="M:ExitGames.Photon.PhotonListener.onEvent(ExitGames.Photon.EventData!System.Runtime.CompilerServices.IsConst*!System.Runtime.CompilerServices.IsImplicitlyDereferenced)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\photon-cpp\inc\photonlistener.h" line="59">
			 This is the event handler function for all Events transmitted by PhotonPeer.
			 @details
			 Whenever a Photon event is sent and received, the receiving peer will be notified via this function. Please refer to @link SendingAndReceivingData Sending and receiving data\endlink for
			 more information.
		 
			 This way, an application can react on any event, based on its event code.
		 
			 The following events are reported by default:
			 EV_RT_JOIN
			 EV_RT_LEAVE
		 
			 These events are predefined and will be triggered as soon as a player has joined or has left the room in which the local player is currently active in. To transmit in-room data, define
			 your own events as needed for your application, and transmit them  using LitePeer::opRaiseEvent().
		 
		 
		 
			 All events which are raised in reaction to some player's actions (like sending data) contain the actor number of the sending player in the "parameters" Hashtable.
		 
			 If the received event has been raised by another player by calling LitePeer::opRaiseEvent(), the transmitted payload hashtable will be stored in the "parameters" hashtable of at key
			 EV_RT_KEY_DATA. Please refer to the demos for sample code.
			 @param eventData the @link EventData\endlink
			 @sa
			 @link SendingAndReceivingData Sending and receiving data\endlink, LitePeer::opRaiseEvent()
</member>
<member name="M:ExitGames.Photon.PhotonListener.onPingResponse(ExitGames.Common.JString!System.Runtime.CompilerServices.IsConst*!System.Runtime.CompilerServices.IsImplicitlyDereferenced,System.UInt32)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\photon-cpp\inc\photonlistener.h" line="85">
			 This is the callback for PhotonPeer::pingServer().
			 @details
			 Each ping signal that has been sent through PhotonPeer::pingServer() results in a call to this function, providing the address to which the ping has been sent and the time in milliseconds
			 that has passed between sending the ping and receiving the servers response.

			 @note: This function is not available on platforms that do not support those parts of the stdlib that have been introduced with C++ 11.
			 @note: Also this function is not available on platforms that do not support multithreading.
			
			 @param address the address, which has been pinged
			 @param pingResult the time in ms
			 @sa
			 @link PhotonPeer::pingServer()\endlink
</member>
<member name="M:PhotonLib.PEventListener.onPhotonEvent(System.Int32,System.Byte,ExitGames.Common.Object!System.Runtime.CompilerServices.IsConst*!System.Runtime.CompilerServices.IsImplicitlyDereferenced)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\peventlistener.h" line="7">
<summary>
<para>他プレイヤーが投げたイベントを受けとる。</para>
<para>使用するにはPNetworkLogicにインスタンスを登録してください。</para>
</summary>
<remarks>
<para>Object型から元の型に戻すには</para>
<para>ValueObject&lt;元の型名&gt;(eventContent).getDataCopy() でコピーを取得</para>
<para>ValueObject&lt;元の型名&gt;(eventContent).getDataAddress() でアドレスを取得</para>
<para>配列の場合、getDataCopy()を使っても配列そのものはコピーされないので別途コピーが必要です。</para>
</remarks>
<param name="playerNr">投げたプレイヤーの番号</param>
<param name="eventCode">イベントの種別</param>
<param name="eventContent">届いたデータ</param>
</member>
<member name="M:PhotonLib.PNetworkLogic.#ctor(ExitGames.Common.JString!System.Runtime.CompilerServices.IsConst*!System.Runtime.CompilerServices.IsImplicitlyDereferenced,ExitGames.Common.JString!System.Runtime.CompilerServices.IsConst*!System.Runtime.CompilerServices.IsImplicitlyDereferenced)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="22">
<summary>
初期化。
</summary>
<param name="appID">photonのサイトで取得したappID</param>
<param name="appVersion">バージョン。適当に決めていい。</param>
</member>
<member name="M:PhotonLib.PNetworkLogic.addEventListener(PhotonLib.PEventListener*)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="30">
<summary>
PEventListenerを継承したクラスを登録する
</summary>
<remarks>
登録したクラスのonPhotonEvent関数に他プレイヤーが投げたイベントが飛んでくる。
</remarks>
<param name="listener"></param>
</member>
<member name="M:PhotonLib.PNetworkLogic.connect" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="41">
<summary>
photonに接続。接続中と接続試行中は何もしない。
</summary>
</member>
<member name="M:PhotonLib.PNetworkLogic.disconnect" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="53">
<summary>
photonから切断。
</summary>
</member>
<member name="M:PhotonLib.PNetworkLogic.getLocalPlayerNum" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="68">
<summary>
自分のプレイヤー番号を取得。
</summary>
<returns></returns>
</member>
<member name="M:PhotonLib.PNetworkLogic.isConnecting" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="76">
<summary>
photon接続状態にあるならtrue。
</summary>
</member>
<member name="M:PhotonLib.PNetworkLogic.isError" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="83">
<summary>
エラーコードが0以外のときtrue。
</summary>
</member>
<member name="M:PhotonLib.PNetworkLogic.isRoomIn" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="90">
<summary>
部屋に入っていればtrue。
</summary>
</member>
<member name="M:PhotonLib.PNetworkLogic.joinOrCreateRoom(ExitGames.Common.JString,System.Byte)" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="97">
<summary>
部屋が既にあれば入室する。無ければ作り、入室する。
</summary>
<param name="roomName">部屋の名前。</param>
<param name="maxPlayer">最大プレイヤー数</param>
</member>
<member name="M:PhotonLib.PNetworkLogic.removeAllListener" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="141">
<summary>
全てのイベントリスナーを登録解除する
</summary>
</member>
<member name="M:PhotonLib.PNetworkLogic.removeListener(PhotonLib.PEventListener*)" decl="true" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="148">
<summary>
イベントリスナーを登録解除する。
</summary>
<param name="listener">登録解除したいリスナー</param>
</member>
<member name="M:PhotonLib.PNetworkLogic.Update" decl="false" source="c:\users\a\kbcfiles\photonlib\photon\photonlib\photon-inc\pnetworklogic.h" line="154">
<summary>
ゲームループ内で呼び出し続けてください
</summary>
</member>
</members>
</doc>