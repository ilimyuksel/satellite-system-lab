# GitHub'a hazırlık

Bu klasör kendi başına depo kökü olacak şekilde hazırlanmıştır. Üst dizindeki
Trick kaynak ağacını, eski eğitim klasörlerini ve belgeler klasörünü ekleme.
Kaynak depo: https://github.com/ilimyuksel/satellite-system-lab . Web yayını henüz yapılmadı.

- Kaynaklar, senaryolar, dokümanlar, testler ve `web/` hazır demosu paylaşılabilir yapıdadır.
- `.gitignore` derlemeleri, ham koşu çıktısını, yerel yolları ve logları dışarıda tutar.
- `config.local.sh` kişiye özeldir; `config.example.sh` örnektir.
- `environment/` içindeki ham JPL verileri tekrarlanabilirlik için saklanır;
  `metadata.json` tarih, çerçeve, kaynak ve sorguyu açıklar.
- Lisans tercihi ekip/proje sahibi tarafından yapılmalıdır. Bu klasöre otomatik
  olarak bir açık kaynak lisansı atanmadı. NASA Trick'in lisansı ayrı kalır.
- `web/index.html` sentetik örnek verisini içerir. Yeni bir gösterim üretilince
  değişikliği bilinçli olarak ekle; HTML büyük bir veri farkı oluşturabilir.
- Çalıştırma ortamı ve doğrulama sınırları README'de belirtilmiştir; Linux'ta henüz test yoktur.

Yayımlamadan önce `git status --short` ve `git diff --cached` ile eklenecekleri incele.
Özellikle `build/`, yürütülebilir dosya ve kişisel mutlak yolların girmediğini kontrol et.
GitHub Pages için yayın kökü `web/` olacak bir yayın düzeni seçilebilir. Canlı Trick
backend'i gerekmez; statik gösterimdir. Şu an yayın ayarı yapılmadı.
