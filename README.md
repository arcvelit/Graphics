# Graphics projects

## Preparation

### 1: Build

#### Linux:

To build the image type the following in a from this directory terminal:

```bash
./linux/build_docker.sh
```

#### Windows:

For Windows use a PowerShell terminal from this directory:

```powershell
./windows/build_docker.ps1
```

**NOTE:** If a permission error occurs, try modifying the execution policy.

```powershell
Set-ExecutionPolicy -ExecutionPolicy Unrestricted -Scope CurrentUser
```

### 2: Set up X Server

#### Windows

[Xming](https://sourceforge.net/projects/xming/) or [VcXsrv](https://sourceforge.net/projects/vcxsrv/)  
**NOTE:** VCXSRV worked with no problem on my machine while XMING created issues.

Then run XLaunch with these configuration options:
1. **Multiple Windows**
2. **Display: 0**
3. **Start no client**
4. **Disable Access Control**

## Running

#### Windows (PowerShell)

Once the docker image is built, call this script in the main directory to run docker.

```powershell
.\dock.ps1
```

### Raytracer

1. Build the raytracer

First, navigate to `raytracer/code/`. Then, execute in order.
```bash
mkdir build
cd build
cmake ../
make
```

Now you can run `./raytracer path/to/json` to raytracer a scene. There are plenty of examples in the assets folder.


### Guitar showcase

1. Build the raytracer

First, navigate to `Project/code/`. Then, execute in order.
```bash
mkdir build
cd build
cmake ../
make
```

Now you can run `./GuitarShowcase` and enjoy the show. You can look in `Project/web` for a demo page.  
Make sure that your XMING or VCXSRV is running properly.
