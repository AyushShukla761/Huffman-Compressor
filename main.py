from fastapi import FastAPI, UploadFile, File
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
import os
import subprocess

app = FastAPI()



UPLOAD_DIR = "uploads"
COMPRESSED_DIR = "compressed"

# Ensure directories exist
os.makedirs(UPLOAD_DIR, exist_ok=True)
os.makedirs(COMPRESSED_DIR, exist_ok=True)

# Detect OS and set the correct executable
cpp_executable = "compressor.exe" if os.name == "nt" else "./compressor"
decomp_cpp_executable = "decompressor.exe" if os.name == "nt" else "./decompressor"

app.mount("/static", StaticFiles(directory="static"), name="static")

@app.get("/")
async def serve_index():
    return FileResponse("static/index.html")

@app.get("/compression")
async def serve_index():
    return FileResponse("static/compression.html")

@app.get("/decompression")
async def serve_index():
    return FileResponse("static/decompression.html")

@app.post("/compress")
async def compress_file(file: UploadFile = File(...)):
    file_path = os.path.join(UPLOAD_DIR, file.filename)
    compressed_path = file_path + ".huff"

    # Save uploaded file
    with open(file_path, "wb") as f:
        f.write(await file.read())

    # Run the C++ compressor
    try:
        result = subprocess.run(
        [cpp_executable, file_path], capture_output=True, text=True, encoding="utf-8", errors="ignore"
    )

        if result.returncode != 0:
            return {"error": "Compression failed", "details": result.stderr}

        compressed_filename = os.path.basename(compressed_path)
        return {"fileName": compressed_filename}

    except Exception as e:
        return {"error": str(e)}

@app.post("/decompress")
async def decompress_file(file: UploadFile= File(...)):
    file_path= os.path.join(UPLOAD_DIR,file.filename)
    print(file_path)
    decompressed_path= os.path.join(UPLOAD_DIR,"decompressed_"+file.filename[:-5])
    print(decompressed_path)
    try:
        data = await file.read()
        with open(file_path, "wb") as f:
            f.write(data)

    except Exception as e:
        return {"error": f"File write failed: {e}"}

    try:
        result = subprocess.run(
        [decomp_cpp_executable, file_path], capture_output=True, text=True, encoding="utf-8", errors="ignore"
    )

        if result.returncode != 0:
            return {"error": "decompression failed", "details": result.stderr}

        decompressed_filename = os.path.basename(decompressed_path)
        return {"fileName": decompressed_filename}

    except Exception as e:
        return {"error": str(e)}


@app.get("/download/{file_name}")
async def download_file(file_name: str):
    file_path = os.path.join(UPLOAD_DIR, file_name)
    if os.path.exists(file_path):
        return FileResponse(file_path, media_type="application/octet-stream", filename=file_name)
    return {"error": "File not found"}
