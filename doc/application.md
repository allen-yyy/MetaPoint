# Application interface

This is the MetaPoint's Application interface guide.

## AppInit

Implemented by application.

MetaPoint will call this interface to initial application.

Return processing results.

```c++
BOOL AppInit(
    void
);
```

## GetAppNum

Implemented by application.

MetaPoint will call this interface to get application port number.

Return application port number.

```c++
unsigned short GetAppNum(
    void
);
```

## OnData

Implemented by application.

MetaPoint will call this interface when there is data.

Return processing results.

```c++
BOOL OnData(
    DWORD length, BYTE *data
);
```

`length` : The data length.

`data` : Data.


## OnNode

Implemented by application.

MetaPoint will call this interface when there is a new node.

Return processing results.

```c++
BOOL OnNode(
    int nodeaddr[4]
);
```

`nodeaddr` : New node's address.

## SendData

Implemented by MetaPoint.

Application will call this interface when there is data want to send.

Return processing results.

```c++
BOOL SendAppData(
    int nodeaddr[4], unsigned short appnum,
    int dwLen, char *cData
);
```

`nodeaddr` : Receiver node's address.

`appnum` : Application number.

`dwLen` : Data length.

`cData` : Data.

## OutInRed

Implemented by MetaPoint.

Application will call this interface to print messages with red.

$\color{red}{Warning: Don't\; use\; printf!}$

No Return.

```c++
void OutInRed(
    string message
);
```

`message` : The message wants to send.

## OutMessage

Implemented by MetaPoint.

Application will call this interface to print messages with black.

$\color{red}{Warning: Don't\; use\; printf!}$

No Return.

```c++
void OutMessage(
    string message
);
```

`message` : The message wants to send.