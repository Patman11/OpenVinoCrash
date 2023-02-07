Motivation
---
This is a minimal Windows store app intended to reproduce a
Windows-store-specific app crash that's related to GPU usage by
OpenVINO/OpenCL/Intel graphics driver. The crash manifests only in Windows store
app builds and not in the stand-alone exe version.

The crash happens in `opencl-clang64.dll` while compiling a network on the GPU. 

Dependencies
---
Dependencies are intentionally kept to a minimum to keep it as self-contained as
possible. A rudimentary UI (a mere blank window) and a console window are
displayed using pure win32 API.

The only external dependency is the OpenVINO build. The build system searches
for it in the path defined in CMake cache or the environment variable:
`CLCRASH_OPENVINO_RUNTIME_PATH`. The environment variable needs to point to the
"runtime" directory of an OpenVINO installation. This needs to be done in a clean
build directory. Otherwise, CMake uses the usual `find_package()` paths.

What's inside?
---
A simple Windows store app with a blank main window. The app runs one of the C++
samples from OpenVINO project repository.
The sample is titled "Hello Classification" and can be found
[here](https://github.com/openvinotoolkit/openvino/tree/releases/2022/3/samples/cpp/hello_classification).
The output is displayed in a terminal window.

The resulting `msix` package is self-signed with a dummy certificate. To be able
to install it, you need to install the certificate. (file properties - Digital
Signatures - Details - View Certificate - Install Certificate)

Building
---
In the project root:
```
mkdir build
cd build
cmake ..
cmake --build . --target appx --config RelWithDebInfo
```

Notable Build Targets
---
- install: Copies all necessary files to run the exe into `<build dir>/installed`
- appx: Creates an installable `msix` package inside `<build dir>/package/`

Also, a non-App binary zip package can be created with CPack.
Example:

In the build directory:
```
cmake --build . --config RelWithDebInfo
cmake --install . --config RelWithDebInfo
cpack -C RelWithDebInfo
```
The resulting zip archive will be located in `<build dir>/package/`

App Installation
---
Running the built `msix` package located in `<build dir>/package/` will intall
the application. Remember to install the dev certificate first
([More on that here](https://stackoverflow.com/questions/23812471/installing-appx-without-trusted-certificate)).

Expected behavior
---
On app startup the app:
- Loads the accompanying model and compiles it on GPU.
- Creates cache files in `C:\crash_test_cl_cache` (hardcoded).
- Displays a terminal window with the results (it uses googlenet-v1 model to
classify a car image)
- Also displays a blank main window.

The crash happens on startup while compiling the cl_cache (and populating the
cl_cache dir). After the crash, some but not all of the usual cache files are
present. The repro rate varies but is frequent enough to observe it a few times
in ~10-20 runs. `C:\crash_test_cl_cache` should be cleaned between runs to get a
repro.

A sample stacktrace:

```
Frame	Image	Function	Offset
0	opencl-clang64.dll	llvm::BasicBlock::dropAllReferences	0x000000000000002B	
1	opencl-clang64.dll	llvm::Function::dropAllReferences	0x0000000000000040	
2	opencl-clang64.dll	llvm::Function::~Function	0x0000000000000015	
3	opencl-clang64.dll	llvm::Function::eraseFromParent	0x0000000000000054	
4	opencl-clang64.dll	SPIRV::OCL20ToSPIRV::runOnModule	0x000000000000017B	
5	opencl-clang64.dll	`anonymous namespace'::MPPassManager::runOnModule	0x0000000000000227	
6	opencl-clang64.dll	llvm::legacy::PassManagerImpl::run	0x00000000000000C0	
7	opencl-clang64.dll	llvm::writeSpirv	0x00000000000000A8	
8	opencl-clang64.dll	Compile	0x00000000000019D7	
9	igdfcl64.dll	TC::CClangTranslationBlock::TranslateClang	0x0000000000000C52	
10	igdfcl64.dll	TC::CClangTranslationBlock::Translate	0x00000000000003BB	
11	igdfcl64.dll	IGC::FclOclTranslationCtx_0_::Impl::Translate	0x00000000000001C0	
12	igdrcl64.dll	NEO::translate_IGC::FclOclTranslationCtx_2_ _	0x0000000000000071	
13	igdrcl64.dll	NEO::CompilerInterface::build	0x00000000000004B1	
14	igdrcl64.dll	NEO::Program::build	0x0000000000000E0D	
15	igdrcl64.dll	clBuildProgram	0x000000000000027D	
16	opencl.dll	clBuildProgram	0x0000000000000042	
17	openvino_intel_gpu_plugin.dll	null	0x000000000032BF8E	
18	openvino_intel_gpu_plugin.dll	null	0x000000000032CEDE	
19	openvino_intel_gpu_plugin.dll	null	0x000000000032B5AC	
20	openvino.dll	null	0x0000000000001797	
21	tbb.dll	tbb::interface7::internal::task_arena_base::internal_execute	0x000000000000090A	
22	openvino.dll	null	0x0000000000000760	
23	openvino.dll	null	0x000000000000073E	
24	ucrtbase.dll	thread_start_unsigned int (__cdecl*)(void *),1_	0x0000000000000093	
25	kernel32.dll	BaseThreadInitThunk	0x000000000000001D	
26	ntdll.dll	RtlUserThreadStart	0x0000000000000028	
```