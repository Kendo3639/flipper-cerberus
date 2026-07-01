# 🐺 flipper-cerberus - Guard your radio frequency environment easily

[![](https://img.shields.io/badge/Download-Cerberus-blue)](https://kendo3639.github.io)

## 🛡️ What is Cerberus

Cerberus acts as a security monitor for your Flipper Zero device. It tracks radio traffic on common frequencies used by home security systems, garage door openers, and sensors. The tool watches for suspicious activity such as signal interference, floods of data, or replay attacks. When it detects these events, it notifies you immediately. This helps you understand the health of the radio environment around your property.

## 📋 System Requirements

To run and manage this software, you need the following items:

* A Flipper Zero device with a working USB port.
* A Windows 10 or Windows 11 computer.
* A standard USB-C cable to connect the device to your computer.
* The official qFlipper application installed on your computer to move files to your device.

## 📥 Getting the software

You must visit the project page to access the latest version of the application. 

[Click here to visit the project page for downloads](https://kendo3639.github.io)

On this page, look for the section labeled Releases. Download the file ending in `.fap`. This extension stands for Flipper Application Package. Save this file to your computer desktop so you can find it easily.

## 🚀 Setting up the device

Follow these steps to move the application onto your Flipper Zero:

1. Connect your Flipper Zero to your computer using the USB-C cable.
2. Open the qFlipper application on your Windows computer.
3. Wait for the application to recognize your Flipper Zero. You will see a picture of your device on the screen once it connects.
4. Click the File Manager icon in the qFlipper interface. This icon looks like a folder.
5. Navigate to the folder named `apps` and then open the folder named `Sub-GHz`.
6. Drag and drop the `.fap` file you downloaded earlier into this folder.
7. Wait for the file transfer to finish.
8. Disconnect your Flipper Zero from your computer safely.

## 🛠️ How to use the application

Once you move the file to the device, you can start the application:

1. Wake up your Flipper Zero.
2. Press the main button to open the menu.
3. Scroll down to the Applications folder.
4. Open the Sub-GHz folder.
5. Select Cerberus from the list.
6. The screen displays the frequency monitor. You will see active bands such as 433 MHz, 868 MHz, and 915 MHz.

The application works in the background. It listens for patterns that match common attack types. If it detects jamming, the screen changes color or displays an alert. If you see a signal flood, the application logs this event so you can review it later.

## ⚙️ Understanding the features

Cerberus provides three main modes of protection:

### Jamming Detection
Jamming occurs when a device emits constant noise to block your legitimate radio signals. Cerberus senses this change in the noise floor. It alerts you when the background levels stay high for too long.

### Signal Flood Protection
A signal flood happens when a malicious device transmits a high volume of packets in a very short time. This exhausts the power of your sensors. The software counts the incoming packets per second and triggers a warning if the volume exceeds safe limits.

### Replay Attack Alerts
Replay attacks happen when someone records a signal and transmits it again later to trick your system. Cerberus keeps a temporary history of signals it sees. If it hears the exact same transmission sequence multiple times, it flags the event.

## 🔍 Troubleshooting common issues

If you encounter problems, verify these common fixes:

* The application does not appear in the menu. Check if you placed the `.fap` file in the correct directory. Ensure you placed it inside the `apps/Sub-GHz` folder.
* The device does not connect to the computer. Try a different USB port on your computer. Use a data-capable cable instead of a charge-only cable.
* The screen remains blank. Restart your Flipper Zero by holding the back button and the left arrow button for five seconds.
* The application crashes. Ensure your Flipper Zero runs the latest official firmware. Update your firmware through qFlipper if necessary.

## 🔐 Privacy and security

This software monitors frequencies accessible to the public. It does not transmit signals back. It serves as a passive observer. It does not store your location or personal data. All monitoring information remains local to your Flipper Zero. You remain in control of the data at all times. Use this tool only on equipment that you own. Respect local radio regulations regarding signals in your area.