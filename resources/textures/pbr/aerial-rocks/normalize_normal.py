import numpy as np
from PIL import Image

def normalize_normal_map(input_path, output_path):
    img = Image.open(input_path).convert("RGB")
    arr = np.asarray(img).astype(np.float32) / 255.0

    # Convert from [0,1] → [-1,1]
    nx = arr[:,:,0] * 2.0 - 1.0
    ny = arr[:,:,1] * 2.0 - 1.0
    nz = arr[:,:,2] * 2.0 - 1.0

    # Reconstruct Z if the map is "flat blue"
    # (most JPG normals have nz ≈ 1.0 everywhere)
    length_xy = nx*nx + ny*ny
    nz = np.sqrt(np.clip(1.0 - length_xy, 0.0, 1.0))

    # Normalize
    length = np.sqrt(nx*nx + ny*ny + nz*nz)
    nx /= length
    ny /= length
    nz /= length

    # Convert back to [0,1]
    out = np.stack([
        (nx * 0.5 + 0.5),
        (ny * 0.5 + 0.5),
        (nz * 0.5 + 0.5)
    ], axis=2)

    out = (out * 255.0).astype(np.uint8)
    Image.fromarray(out).save(output_path)

normalize_normal_map("aerial_rocks_04_nor_gl_2k.jpg", "normalized.png")


#and after
#toktx --uastc --uastc_quality 2 --assign_oetf linear --zcmp 18 normalized.ktx2 normalized.png



