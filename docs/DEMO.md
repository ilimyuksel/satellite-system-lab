# Mentöre 3–5 dakikalık gösterim

## Söylenebilecek somut özet

> NASA Trick üzerinde çalışan bir C++ yörünge/çevre modelini basit elektrik güç
> sistemi ve faydalı yükle birleştirdik. Gölgeye giriş panel üretimini, üretim ve
> yükler batarya enerjisini, enerji durumu da faydalı yükün çalışmasını etkiliyor.
> İki senaryoyu çalıştırdık ve enerji dengesi ile sınır davranışlarını test ettik.

## Gösterim sırası

1. README kapsam tablosunu göster; ADCS ve termalin henüz olmadığını belirt.
2. `./tools/run.sh nominal` çalıştır. Çıktıdaki `web/index.html` dosyasını aç.
3. Oynat'a bas. Küreyi döndür; bunun kaydedilmiş Trick sonuçları olduğunu söyle.
4. Güneş görünürlüğü, panel üretimi, güç grafiği ve SOC grafiğini birlikte izle.
5. Faydalı yükün zaman planıyla beklemeden çalışmaya geçmesini göster.
6. `./tools/run.sh low_power` çalıştırıp sekmeyi yenile. Düşük enerji/yük kısıtlamasını göster.
7. `docs/VALIDATION.md` ve `artifacts/system-validation.json` sonuçlarını göster.
8. Sonraki adım olarak termal bağ ve hesaplanan panel yönelimini açıkla.

## Söylenmemesi gerekenler

- "Gerçek uydu telemetrisi kullandık." → Veriler sentetik.
- "Tüm uydu sistemlerini modelledik." → İlk alt sistem bağlantıları var.
- "3B gövdenin yönelimi ADCS sonucu." → Gövde simgesi temsili.
- "Batarya modeli hücre seviyesinde doğru." → Enerji deposu yaklaşımı.
- "Yapay zekâ anomalileri buluyor." → Henüz bu aşamada değiliz.
- "Oynat dediğimizde Trick canlı çalışıyor." → Tamamlanmış koşu oynatılıyor.

## Kodun nereden anlatılacağı

`S_define` → `models/system/src/Spacecraft.cpp` → `models/power/src/Power.cpp`
→ `RUN_nominal/input.py` → `record.py` sırası, fizik ve yazılım bağlantısını açıklar.
