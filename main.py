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

app.mount("/static", StaticFiles(directory="static"), name="static")

@app.get("/")
async def serve_index():
    return FileResponse("static/index.html")


@app.post("/compress")
async def compress_file(file: UploadFile = File(...)):
    file_path = os.path.join(UPLOAD_DIR, file.filename)
    compressed_path = file_path + ".hdta"

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

@app.get("/download/{file_name}")
async def download_file(file_name: str):
    file_path = os.path.join(UPLOAD_DIR, file_name)
    if os.path.exists(file_path):
        return FileResponse(file_path, media_type="application/octet-stream", filename=file_name)
    return {"error": "File not found"}
