# OpenBSC Protocol -- Documentation

## 1. Purpose

This protocol implements a serial communication scheme inspired by **IBM
BSC (Binary Synchronous Communications)**.\
Its goal is to ensure **data integrity** and **flow control** through
message framing, block check character (BCC), and ACK/NAK feedback.

------------------------------------------------------------------------

## 2. Frame Structure

Every command sent by the application is encapsulated in the following
format:

  Field         Value           Description
  ------------- --------------- --------------------------------------
  **SOH**       0x01            Start of header (optional, reserved)
  **STX**       0x02            Start of data (payload)
  **Payload**   ASCII/Bytes     Command or data being transmitted
  **ETX**       0x03            End of data
  **BCC**       XOR(STX..ETX)   Block Check Character (integrity)

### Example

Command to send: `"LED_ON"`

Frame transmitted:

    0x01 0x02 'L' 'E' 'D' '_' 'O' 'N' 0x03 <BCC>

------------------------------------------------------------------------

## 3. Device Responses

After receiving a frame, the device must reply with:

-   **ACK (0x06)** → Command accepted and processed successfully.\
-   **NAK (0x15)** → Command rejected (framing error or invalid BCC).

If the device also needs to return data (e.g., status response), it must
send another valid frame following the same structure.

------------------------------------------------------------------------

## 4. Host Side (PC / SDK)

### 4.1 Sending (`SendCommand`)

1.  Build the frame according to Section 2.\
2.  Transmit it over the serial interface.\
3.  Wait for response:
    -   `ACK` → Success.\
    -   `NAK` → Failure, may retransmit.\
    -   **Timeout** → Failure (no response).

### 4.2 Receiving (`ReadResponse`)

1.  Read raw bytes from the serial port.\
2.  Locate **STX..ETX**.\
3.  Compute **BCC** and compare with received one.\
4.  If valid → return only the **payload**.\
5.  If invalid → discard.

------------------------------------------------------------------------

## 5. Device Side (Firmware)

The device must implement the same protocol to establish communication.

### 5.1 Reception

1.  Read incoming bytes.\
2.  Validate framing:
    -   Ensure presence of `STX` and `ETX`.\
    -   Check length constraints.\
3.  Compute BCC.\
4.  If valid:
    -   Send `ACK (0x06)`.\
    -   Process payload.\
5.  If invalid:
    -   Send `NAK (0x15)`.

### 5.2 Transmission

When sending back data (e.g., sensor values), the device must frame it
as:

    SOH (0x01) | STX (0x02) | <payload> | ETX (0x03) | BCC

------------------------------------------------------------------------

## 6. Operating Modes

If the device **does not** support this protocol, the host can work in
**RAW mode**, transmitting plain strings without framing.

  Mode      Description
  --------- -------------------------------------------------
  **RAW**   Sends/receives plain text (compatibility mode).
  **BSC**   Full framing with SOH/STX/ETX/BCC and ACK/NAK.

------------------------------------------------------------------------

## 7. Advantages of BSC Mode

-   Error detection with **BCC**.\
-   Flow control with **ACK/NAK**.\
-   Clear framing of messages.\
-   Possibility of **automatic retransmission**.

------------------------------------------------------------------------

## 8. Requirements

-   **Host (PC/SDK):** Implemented in the `OpenBSC` library.\
-   **Device (firmware):** Must be programmed to parse the framing,
    verify BCC, and reply with ACK/NAK.

------------------------------------------------------------------------