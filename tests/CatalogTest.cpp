 /****************************************************************************************
  * Copyright (c) 2010 Leo Franchi <lfranchi@kde.org>                                    *
  *                                                                                      *
  * This program is free software; you can redistribute it and/or modify it under        *
  * the terms of the GNU General Public License as published by the Free Software        *
  * Foundation; either version 2 of the License, or (at your option) any later           *
  * version.                                                                             *
  *                                                                                      *
  * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
  * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
  * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
  *                                                                                      *
  * You should have received a copy of the GNU General Public License along with         *
  * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
  ****************************************************************************************/
 
#include "CatalogTest.h"

#include "Config.h"
#include "Catalog.h"

#include <QDebug>
#include <QNetworkReply>

void CatalogTest::initTestCase()
{
    Echonest::Config::instance()->setAPIKey( "JGJCRKWLXLBZIFAZB" );
}

void CatalogTest::testList()
{
    QNetworkReply* reply = Echonest::Catalog::list();
    
    qDebug() << reply->url().toString();
    QCOMPARE( reply->url().toString(), QLatin1String( "http://developer.echonest.com/api/v4/catalog/list?api_key=JGJCRKWLXLBZIFAZB&format=xml" ) );
    
    QEventLoop loop;
    loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
    loop.exec();
    
    Echonest::Catalogs catalogs = Echonest::Catalog::parseList( reply );
    qDebug() << catalogs.size();
    qDebug() << catalogs;
    QVERIFY( !catalogs.isEmpty() );
    
    // Now limit it to just 1, but the second one.
    reply = Echonest::Catalog::list( 1, 1 );
    
    qDebug() << reply->url().toString();
    QCOMPARE( reply->url().toString(), QLatin1String( "http://developer.echonest.com/api/v4/catalog/list?api_key=JGJCRKWLXLBZIFAZB&format=xml&results=1&start=1" ) );
    
    loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
    loop.exec();
    
    catalogs = Echonest::Catalog::parseList( reply );
    qDebug() << catalogs.size();
    QCOMPARE( catalogs.size(), 1 );
}

void CatalogTest::testProfile()
{
    Echonest::Catalog c( "CAWRKLJ12BF92BC7C3" );
    QNetworkReply* reply = c.profile();
    
    qDebug() << reply->url().toString();
    QCOMPARE( reply->url().toString(), QLatin1String( "http://developer.echonest.com/api/v4/catalog/profile?api_key=JGJCRKWLXLBZIFAZB&format=xml&id=CAWRKLJ12BF92BC7C3" ) );
    
    QEventLoop loop;
    loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
    loop.exec();
    
    c.parseProfile( reply );
    qDebug() << c;
    QCOMPARE( c.total(), 3 );
}

void CatalogTest::testRead()
{
    Echonest::Catalog c( "CAWRKLJ12BF92BC7C3" );
    QNetworkReply* reply = c.readSongCatalog( Echonest::Song::AudioSummaryInformation | Echonest::Song::Tracks | Echonest::Song::Hotttnesss | Echonest::Song::ArtistHotttnesss | Echonest::Song::ArtistFamiliarity | Echonest::Song::ArtistLocation );
    
    qDebug() << reply->url().toString();
    QCOMPARE( reply->url(), QUrl( QLatin1String( "http://developer.echonest.com/api/v4/catalog/read?api_key=JGJCRKWLXLBZIFAZB&format=xml&bucket=audio_summary&bucket=tracks&bucket=song_hotttnesss&bucket=artist_hotttnesss&bucket=artist_familiarity&bucket=artist_location&id=CAWRKLJ12BF92BC7C3" ) ) );
    
    QEventLoop loop;
    loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
    loop.exec();
    
    c.parseRead( reply );
    qDebug() << c;
    QVERIFY( !c.songs().isEmpty() );
    QVERIFY( c.songs().at( 0 ).audioSummary().duration() > 0 );
    QVERIFY( c.songs().at( 0 ).audioSummary().danceability() > 0 );
    QVERIFY( c.songs().at( 0 ).audioSummary().energy() > 0 );
//     QVERIFY( !c.songs().at( 0 ).tracks().isEmpty() );
    QVERIFY( c.songs().at( 0 ).hotttnesss() > 0 );
    QVERIFY( !c.songs().at( 0 ).dateAdded().isNull() );
    QVERIFY( !c.songs().at( 0 ).artistId().isEmpty() );
    QVERIFY( !c.songs().at( 0 ).foreignId().isEmpty() );
    QVERIFY( c.songs().at( 0 ).artistHotttnesss() >= 0 );
    QVERIFY( c.songs().at( 0 ).artistFamiliarity() >= 0 );
    
    // test an artist catalog
    Echonest::Catalog c2( "CAXBXBZ12BF92A9AC2" );
    reply = c2.readArtistCatalog( Echonest::Artist::Audio | Echonest::Artist::Blogs | Echonest::Artist::Biographies | Echonest::Artist::Familiarity |
                                                Echonest::Artist::Hotttnesss | Echonest::Artist::Images | Echonest::Artist::News | Echonest::Artist::Reviews | 
                                                Echonest::Artist::Terms | Echonest::Artist::Urls | Echonest::Artist::Videos );
    
    qDebug() << reply->url().toString();
    QCOMPARE( reply->url(), QUrl( QLatin1String( "http://developer.echonest.com/api/v4/catalog/read?api_key=JGJCRKWLXLBZIFAZB&format=xml&bucket=audio&bucket=biographies&bucket=blogs&bucket=familiarity&bucket=hotttnesss&bucket=images&bucket=news&bucket=reviews&bucket=terms&bucket=urls&bucket=video&id=CAXBXBZ12BF92A9AC2" ) ) );
    
    loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
    loop.exec();
    
    c2.parseRead( reply );
    qDebug() << c2;
    QVERIFY( !c2.artists().isEmpty() );
    QVERIFY( c2.artists().at( 0 ).familiarity() > 0 );
    QVERIFY( !c2.artists().at( 0 ).terms().isEmpty() );
    QVERIFY( !c2.artists().at( 0 ).biographies().isEmpty() );
    QVERIFY( !c2.artists().at( 0 ).blogs().isEmpty() );
    QVERIFY( !c2.artists().at( 0 ).name().isEmpty() );
    QVERIFY( c2.artists().at( 0 ).hotttnesss() > 0 );
    QVERIFY( !c2.artists().at( 0 ).reviews().isEmpty() );
    QVERIFY( !c2.artists().at( 0 ).videos().isEmpty() );
    QVERIFY( c2.artists().at( 0 ).lastFmUrl().isValid() );
    QVERIFY( !c2.artists().at( 0 ).news().isEmpty() );
    QVERIFY( !c2.artists().at( 0 ).images().isEmpty() );
    QVERIFY( !c2.artists().at( 0 ).audio().isEmpty() );
    QVERIFY( !c2.artists().at( 0 ).dateAdded().isNull() );
    QVERIFY( !c2.artists().at( 0 ).id().isEmpty() );
    QVERIFY( !c2.artists().at( 0 ).foreignId().isEmpty() );
}

void CatalogTest::testStatus()
{

}

void CatalogTest::testCreateUpdateDeleteSong()
{
    Echonest::Catalog c;
    try {
        QNetworkReply* reply = Echonest::Catalog::create( QLatin1String( "unittest_catalog_song_X" ), Echonest::CatalogTypes::Song );
//         
        QEventLoop loop;
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        
        c = Echonest::Catalog::parseCreate( reply );
        qDebug() << "CREATED NEW CATALOG:" << c;
        QVERIFY( !c.id().isEmpty() );
        QVERIFY( !c.name().isEmpty() );
//         c = Echonest::Catalog( "CAQDANH12BFA210FC4" ); // for deleting manually a catalog
        Echonest::CatalogUpdateEntry entry;
        entry.setSongName( QLatin1String( "Your Hand In Mine" ) );
        entry.setArtistName( QLatin1String( "Explosions in the sky" ) );
    //     entry.S( "fooid" );
    //     entry.setFingerpring( "FINGERPRINT" );
    //     entry.setRelease( QLatin1String( "FooAlbum:" ) );
    //     entry.setGenre( QLatin1String( "Rock" ) );
    //     entry.setRating( 5 );
    //     entry.setTrackNumber( 5 );
    //     entry.setDiscNumber( 1 );
    //     entry.setUrl( QLatin1String( "myurl" ) );
    //     entry.setFavorite( true );
        entry.setAction( Echonest::CatalogTypes::Update );
        Echonest::CatalogUpdateEntries entries;
        entries << entry;
        
        reply = c.update( entries );
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        
        QByteArray ticket = Echonest::Catalog::parseTicket( reply );
        qDebug() << ticket;
        
        QVERIFY( !ticket.isEmpty() );
        // now check the ticket status after 5s
        sleep( 5 );
        reply = Echonest::Catalog::status( ticket );
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        Echonest::CatalogStatus cs = Echonest::Catalog::parseStatus( reply );
        qDebug() << "Catalog status:" << cs.status << cs.items_updated << cs.items;
        
        // now read the catalog
        reply = c.readSongCatalog();
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        c.parseRead( reply );
        qDebug() << c;
        QCOMPARE( c.songs().size(), 1 );
    } catch( const Echonest::ParseError& e ) { // delete no matter what
        qDebug() << "Caught exception, deleting anyway!";
        
        QNetworkReply* reply = c.deleteCatalog();
        QEventLoop loop;
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        return;
    }
    QNetworkReply* reply = c.deleteCatalog();
    QEventLoop loop;
    loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
    loop.exec();
    QPair< QString, QByteArray> res = c.parseDelete( reply );
    qDebug() << res;
    
}

void CatalogTest::testCreateUpdateDeleteArtist()
{
    Echonest::Catalog c;
    try {
        QNetworkReply* reply = Echonest::Catalog::create( QLatin1String( "unittest_catalog_artist_X" ), Echonest::CatalogTypes::Artist );
//         
        QEventLoop loop;
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        
        c = Echonest::Catalog::parseCreate( reply );
        qDebug() << "CREATED NEW ARTIST CATALOG:" << c;
        QVERIFY( !c.id().isEmpty() );
        QVERIFY( !c.name().isEmpty() );

//         c = Echonest::Catalog( "CAPRWVK12BFA1A6F17" );
        
        Echonest::CatalogUpdateEntry entry;
        entry.setArtistName( QLatin1String( "Balmorhea" ) );
        entry.setGenre( QLatin1String( "Post Rock" ) );
        entry.setAction( Echonest::CatalogTypes::Update );
        Echonest::CatalogUpdateEntry entry2;
        entry2.setArtistName( QLatin1String( "Mono" ) );
        entry2.setGenre( QLatin1String( "Post Rock" ) );
        entry2.setAction( Echonest::CatalogTypes::Update );
        Echonest::CatalogUpdateEntries entries;
        entries << entry << entry2;
        
        reply = c.update( entries );
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        
        QByteArray ticket = Echonest::Catalog::parseTicket( reply );
        qDebug() << ticket;
        
        QVERIFY( !ticket.isEmpty() );
        // now check the ticket status after 5s
        sleep( 5 );
        reply = Echonest::Catalog::status( ticket );
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        Echonest::CatalogStatus cs = Echonest::Catalog::parseStatus( reply );
        qDebug() << "Catalog status:" << cs.status << cs.items_updated << cs.items;
        
        // now read the catalog
        reply = c.readSongCatalog();
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        c.parseRead( reply );
        qDebug() << c;
        QCOMPARE( c.artists().size(), 2 );
    } catch( const Echonest::ParseError& e ) { // delete no matter what
        qDebug() << "Caught exception, deleting anyway!";
        
        QNetworkReply* reply = c.deleteCatalog();
        QEventLoop loop;
        loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
        loop.exec();
        return;
    }
    QNetworkReply* reply = c.deleteCatalog();
    QEventLoop loop;
    loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
    loop.exec();
    QPair< QString, QByteArray> res = c.parseDelete( reply );
    qDebug() << res;
}


QTEST_MAIN(CatalogTest)

#include "CatalogTest.moc"
