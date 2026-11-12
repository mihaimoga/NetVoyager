![NetVoyager1.png](NetVoyager.png)

# NetVoyager
NetVoyager is a lightweight network diagnostics app that lets users quickly assess connectivity and trace routes. With built-in ping and traceroute tools, it helps identify latency issues, unreachable hosts, and visualize packet paths. Ideal for IT pros, engineers, and curious users, with a clean UI and real-time results.

## ✨ Features
- **Ping and traceroute** over IPv4 or IPv6.
- **Live diagnostic results**, including latency, packet-loss summaries, and per-hop route details.
- **Optional reverse DNS lookups** to display hostnames for responding IP addresses.
- **Windows desktop interface** built with MFC and WebView2.

## 📦 Installation
Download and run the [NetVoyager installer](https://www.moga.doctor/freeware/NetVoyagerSetup.msi). NetVoyager uses the Microsoft Edge WebView2 Runtime; if it is not already installed, get it from the [WebView2 download page](https://developer.microsoft.com/microsoft-edge/webview2/).

## 🛠️ Build (MSBuild / Visual Studio)

### Requirements
- **Windows 10 or later**
- **Visual Studio 2026** with the **Desktop development with C++** workload and MFC components
- **MSVC v145** and the **Windows 10 SDK**
- **NuGet** for restoring the native dependencies

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/mihaimoga/NetVoyager.git
   cd NetVoyager
   ```
2. Restore NuGet packages with `nuget restore NetVoyager.sln`.
3. Open `NetVoyager.sln` in Visual Studio and select **Release | x64** (the solution also includes Win32 configurations).
4. Build with **Build → Build Solution** (`Ctrl+Shift+B`).
-   Output binary: ```.\x64\Release\NetVoyager.exe```

## 🖥️ Using NetVoyager

1. Launch NetVoyager and choose **Ping** or **Traceroute** from the **Home** tab.
2. Enter a hostname or IP address in the prompt.
3. Choose whether to resolve responding IP addresses to hostnames and whether to use IPv6.
4. Review the live ping statistics or traceroute hop results in the main window.

![NetVoyager ping results](NetVoyager-Ping.png)

![NetVoyager traceroute results](NetVoyager-Traceroute.png)

## 🤝 Contributing

1. Fork the repo
2. Create a branch: ```git checkout -b feature/new-probe```
3. Commit: ```git commit -m "Add new probe"```
4. Push: ```git push origin feature/new-probe```
5. Open a PR

## 🗃️ License
This project is licensed under the GNU General Public License v3.0 - see the LICENSE file for details.

## 🙋 FAQ

**Q: Does NetVoyager support IPv6?**
A: Yes. Select the IPv6 option when starting Ping or Traceroute.

**Q: Which platforms are supported?**
A: Windows 10 or later. The solution provides both x64 and Win32 build configurations.

**Q: Does NetVoyager need WebView2?**
A: Yes. The Microsoft Edge WebView2 Runtime is required to display the application interface; install it from the [WebView2 download page](https://developer.microsoft.com/microsoft-edge/webview2/) if needed.

## 🛣️ Roadmap (ideas)

- Export results to JSON or CSV.
- Dark theme and accessibility improvements.
- Continuous Integration (GitHub Actions) for automated builds.
