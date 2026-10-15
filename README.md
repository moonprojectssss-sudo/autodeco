# Auto Deco (Geode) — Made by XYS

Mod Geode buat Geometry Dash. Kamu bikin layout di level editor, tekan tombol **Deco**
(di samping tombol trash), pilih tema + mood, lalu deco otomatis ditempel ke layout.

> Status: **v1.0 (kerangka lengkap, belum dites compile)**. Ditulis mengacu docs Geode v5.x
> (versi terbaru yang aku temukan waktu nulis: 5.11.1). Kalau build error, lihat bagian "Kalau error".

---

## 1. Fitur

- Tombol **Deco** di editor, nempel di samping tombol trash.
- **Tema:** Space, Slaughter, Neon, Forest, Cyber.
- **Mood:** Clean, Slaughterhouse, Limbo, Dreamy (ngatur kepadatan, ukuran, rotasi acak).
- **Density:** Low / Medium / High.
- **Beat sync:** isi BPM + kecepatan level. Deco besar ditaruh per 1 bar (4 ketukan),
  dan ada "rel" aksen di atas dan bawah layout tiap ketukan (ketukan pertama tiap bar lebih besar).
- **Palette sendiri:** kamu pilih deco favorit di editor, mod menyimpannya per tema dan per slot.
- **Aman:** objek yang ternyata bukan deco (blok, spike, portal) otomatis dihapus lagi, jadi level gak jadi mustahil.
- Hasil deco otomatis ke-**select**. Gak suka? Tekan Trash.

## 2. Cara pakai (di editor)

1. Bikin layout (blok, spike, dll).
2. **Pertama kali:** isi palette dulu (lihat bagian 3).
3. Tekan tombol **Deco** di samping Trash.
4. Pilih Tema, Mood, Density. Mau ikut lagu? Nyalain **Beat Sync**, isi **BPM**, pilih **Kecepatan** level.
5. Tekan **Generate!**
6. Kalau ada objek yang kamu select sebelum tekan Generate, deco cuma dibuat buat objek itu.
   Kalau tidak ada yang di-select, deco dibuat buat seluruh layout.

## 3. Palette (penting, ini yang bikin hasilnya bagus)

Mod ini gak bawa daftar ID deco bawaan, karena ID deco harus dicek langsung di game.
Jadi kamu yang milih "bahan" deco-nya:

1. Di editor, taruh/select beberapa objek **deco** yang cocok buat tema, misalnya bintang/planet buat Space.
2. Buka popup **Deco**, pilih Tema yang sesuai.
3. Tekan slot-nya:
   - **Edge** = deco kecil di atas permukaan blok (rumput, duri deco, dll)
   - **Hang** = deco yang menggantung di bawah blok (rantai, stalaktit, dll)
   - **BG** = deco besar di latar belakang
   - **Beat** = aksen yang muncul tiap ketukan
4. Slot yang kosong otomatis pinjam slot lain di tema yang sama. Minimal isi **Edge** satu kali.
5. **Reset** = kosongin palette tema yang lagi dipilih.

Palette tersimpan otomatis, jadi cukup sekali per tema.

## 4. Cara build

### A. Di HP (Android + Termux) — pakai GitHub yang nge-build

Build Geode di HP berat banget, jadi yang nge-build GitHub Actions (gratis). Kamu cuma upload.

```bash
# 1) siapin Termux (sekali aja)
pkg update -y && pkg install -y git gh unzip
termux-setup-storage

# 2) ambil zip ini dari Download, lalu ekstrak
cd ~
unzip ~/storage/downloads/AutoDeco.zip
cd AutoDeco

# 3) upload ke GitHub (login lewat browser, ikuti petunjuk di layar)
git init -b main
git add .
git commit -m "Auto Deco v1"
gh auth login
gh repo create autodeco --public --source=. --push

# 4) tunggu build selesai (sekitar 5 sampai 15 menit), lalu download hasilnya
gh run watch
gh run download -n "Build Output"

# 5) pasang ke Geode (cek dulu foldernya ada; kalau belum, buka GD/Geode sekali)
cp *.geode ~/storage/shared/Android/media/com.geode.launcher/game/geode/mods/
```

Setelah itu buka ulang Geometry Dash. Kalau build gagal, kirim hasil ini ke aku:
`gh run view --log-failed`

Tanpa Termux juga bisa: upload folder ke GitHub lewat browser, buka tab Actions,
download artifact "Build Output".

### B. Di laptop (Windows/Mac/Linux)

1. Install Geode CLI dan SDK: ikuti https://docs.geode-sdk.org/getting-started/
2. Di folder `AutoDeco`:

```bash
geode build
```

`geode build` bikin file `.geode` dan memasangnya ke Geometry Dash kamu.
Cek versi SDK kamu dengan `geode sdk version`, lalu samain angka `"geode"` di `mod.json`.

## 5. Kalau error

Kerangka ini ditulis tanpa bisa compile di sini, jadi kemungkinan ada nama fungsi yang beda di SDK kamu.
Titik yang paling mungkin bermasalah:

| Pesan error menyebut | Perbaikan |
|---|---|
| `m_zLayer` / `ZLayer::B2` | hapus baris `obj->m_zLayer = z;` di `src/Engine.hpp` (deco jadi layer default, bisa kamu ubah manual) |
| `updateCustomScaleX` | ganti jadi `obj->setScale(scale);` |
| `Popup::init` | cek signature di https://docs.geode-sdk.org/classes/geode/Popup |
| `createTogglerWithStandardSprites` | ganti jadi `CCMenuItemToggler::createWithStandardSprites(this, nullptr, 0.55f)` |
| `"geode"` di mod.json | samain dengan versi SDK terpasang |
| Tombol Deco gak nempel di samping trash | ID tombol trash beda; ada tombol cadangan di kiri atas. Kirim screenshot ke aku |

## 6. Struktur file

```
AutoDeco/
├── mod.json              info mod (nama, versi, developer XYS)
├── CMakeLists.txt        build
├── about.md, changelog.md
├── .github/workflows/build.yml   build otomatis di GitHub
└── src/
    ├── main.cpp          nambahin tombol Deco di editor
    ├── DecoPopup.hpp     GUI
    ├── Engine.hpp        otak Auto Deco (nempatin deco)
    └── Themes.hpp        daftar tema, mood, kecepatan (gampang diedit)
```

Mau nambah tema baru? Tambah satu baris di `kThemes` (dan ubah angka `std::array<Theme, N>`) di `Themes.hpp`.

## 7. Belum ada (rencana)

- Warna otomatis per tema (butuh color trigger)
- Pulse trigger otomatis di tiap ketukan
- Deteksi beat langsung dari lagu (sekarang manual lewat BPM)
- Dukungan speed portal di tengah level (sekarang satu kecepatan buat seluruh level)

Made by XYS
