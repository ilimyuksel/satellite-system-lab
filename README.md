# Satellite System Lab

**NASA Trick üzerinde C++ ile geliştirilen uydu sistem modelinin ilk bütünleşik sürümü.**

Bu proje, yörünge/çevre modelini elektrik güç sistemi ve basit bir faydalı yükle
birleştirir. Simülasyon gerçek Trick koşusudur; web ekranı bu koşudan kaydedilen
veriyi 3B olarak oynatır. Veriler ve uydu parametreleri eğitim amaçlı sentetiktir.

**Mevcut aşama: M1 — Yörünge + çevre + EPS + faydalı yük bağlantısı.**
ADCS, termal sistem, hata enjeksiyonu ve yapay zekâ henüz uygulanmadı.

## Şu an ne gösterebiliyoruz?

- Dünya çevresinde üç boyutlu, sayısal olarak integre edilen uydu yörüngesi.
- Dünya gölgesinin güneş paneli üretimini azaltması/kesmesi.
- Panel üretimi ile yük tüketimi arasındaki farkın bataryaya etkisi.
- Zaman planına göre faydalı yükün çalışması; düşük enerjide engellenmesi.
- Batarya doluluk sınırları, kullanılmayan güç ve karşılanamayan güç talebi.
- Kuvvet ve enerji dengesi testleri ile tekrarlanabilir iki senaryo.
- 3B yörünge, batarya doluluğu ve güç eğrilerini aynı zamanda gösteren web demo.

**[Hazır 3B demoyu aç](web/index.html)** · **[Özelliklerin durumu](docs/STATUS.md)** ·
**[Model mimarisi](docs/ARCHITECTURE.md)** · **[Parametreler](docs/PARAMETERS.md)** ·
**[Mentör gösterimi](docs/DEMO.md)** · **[Doğrulama sonuçları](docs/VALIDATION.md)**

GitHub dosya görünümü HTML'i uygulama olarak çalıştırmaz. Yerelde tarayıcıda aç
veya ileride yalnızca `web/` klasörünü GitHub Pages ile yayımla. Şu anda çevrimiçi
web yayını yapılmadı. Kaynak depo: [ilimyuksel/satellite-system-lab](https://github.com/ilimyuksel/satellite-system-lab).

## Hızlı başlangıç

### Yalnızca hazır sonucu görmek

Trick kurmadan `web/index.html` dosyasını tarayıcıda aç. Alternatif:

```sh
python3 tools/serve.py
# http://127.0.0.1:8765
```

Bu sunucu sadece `web/` klasörünü localhost üzerinde sunar. Oynat, simülasyonu
başlatmaz. Ekrandaki veriler kayıt oynatımıdır.

### Modeli yeniden çalıştırmak

Gereksinimler: çalışan NASA Trick kurulumu, onunla uyumlu C++ derleyicisi ve Python.
Bu sürüm macOS Apple Silicon / Trick 25.1.2-beta üzerinde çalıştırıldı; Linux
komutları taşınabilir olacak şekilde yazıldı ancak Linux'ta test edilmedi.

```sh
# Depo kökünden; Trick zaten PATH'te ise bu satır gerekli olmayabilir:
export TRICK_HOME=/path/to/trick
./tools/run.sh nominal
```

Bu çalışma alanında komşu `../trick` kurulumu otomatik bulunur. Başka bilgisayarda
`config.example.sh` dosyasını `config.local.sh` olarak kopyalayıp yerel yolları
ayarlayabilirsin. Yerel ayarlar Git'e dahil edilmez.

```sh
./tools/run.sh low_power  # düşük başlangıç enerjisi ve küçük etkin panel alanı
./tools/run.sh nominal    # normal demoyu yeniden üret
```

Her koşu `web/index.html` içeriğini seçilen senaryoyla yeniler. Açık sekmeyi yenile.
C++ değişiklikleri artımlı derlenir. Hata durumunda `artifacts/build.log` veya
`artifacts/<senaryo>.log` dosyasını incele.

### Kontroller

```sh
# NumPy/SciPy yalnızca bağımsız referans doğrulaması için gerekli:
python3 -m pip install -r requirements-validation.txt
./tools/check.sh
```

Özel bir Python ortamı kullanıyorsan Trick'in gömülü Python sürümüyle uyumluluğunu
koru. Test betiği yerel Trick ortamını `tools/environment.sh` üzerinden seçer.

## Proje yapısı

```text
satellite-system-lab/
├── README.md
├── docs/                       # amaç, kapsam, mimari, parametre ve test açıklamaları
├── sim/SIM_satellite/
│   ├── S_define                # Trick iş zamanlaması ve model bağlantıları
│   ├── models/
│   │   ├── orbit/              # yörünge ve dış çevre kuvvetleri
│   │   ├── power/              # EPS güç/enerji dengesi
│   │   ├── payload/            # komut planı ve güç talebi
│   │   └── system/             # alt sistemlerin birlikte çalışması
│   ├── environment/            # paketli NASA/JPL verileri ve kaynak bilgisi
│   ├── RUN_nominal/input.py    # ilk gösterim senaryosu
│   ├── RUN_low_power/input.py  # düşük enerji senaryosu
│   └── record.py               # telemetri kayıt listesi
├── tools/                      # derleme, koşu, doğrulama ve web aktarımı
├── tests/                      # C++ ve Python kontrolleri
├── web/                        # bağımsız 3B demo ve şablon
└── artifacts/                  # yerel loglar/raporlar; Git'e alınmaz
```

## İlk doğrulanmış sonuçlar

200 dakikalık senaryo; 80 Wh kullanılabilir batarya kapasitesi:

| Ölçüm | Nominal | Düşük enerji |
|---|---:|---:|
| Başlangıç doluluğu | %65 | %22 |
| Son doluluk | %33,35 | %0 |
| Faydalı yükün gerçek çalışma süresi | 6000 s | 0 s |

Düşük enerji koşusu başarısız bir yazılım koşusu değildir: üretim/tüketim dengesi
yetersiz seçilmiştir. Uydu modeli depolama enerjisini sıfırın altına indirmeden
karşılanamayan güç talebini kaydeder. Platform kapanma/yeniden başlama elektroniği
henüz modellenmez; fizik ve kayıt işlemleri devam eder.

## Modelin sınırı

EPS, sabit verimli ve sabit kapasiteli bir enerji deposudur; hücre gerilimi,
iç direnç, sıcaklık etkisi veya yaşlanma modeli değildir. Panel açısı şu an
**verilen bir katsayıdır**, ADCS tarafından hesaplanmaz. Faydalı yük yalnızca
çalışma modu ve güç talebidir; görüntü üretimi, veri hacmi ve termal hesap yoktur.

Yörünge modeli J2, nominal üstel atmosfer, ışınım basıncı ve Güneş/Ay çekimi içerir.
Bunlar tüm ortam etkilerinin yüksek hassasiyetle modellenmiş olduğu anlamına gelmez.
Güneş/Ay verisi 16–18 Ekim 2026 **TDB** aralığıyla sınırlıdır. Ayrıntılar
[model varsayımlarında](docs/ARCHITECTURE.md).

## Köken ve paylaşım

Bu klasör önceki `SIM_satellite_learning` prototipinden doğrulanmış yörünge ve
çevre parçalarını taşır; önceki örneği değiştirmez. NASA Trick kaynak kodunu,
derleme ürünlerini veya kişisel sistem ayarlarını depoya dahil etmeyin.
[Kaynaklar ve atıflar](NOTICE.md) ve [paylaşım kontrol listesi](docs/SHARING.md).
