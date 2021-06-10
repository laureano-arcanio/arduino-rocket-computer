///* 
//  Never Gonna Give you Up
//  Connect a piezo buzzer or speaker to pin 11 or select a new pin.
//  More songs available at https://github.com/robsoncouto/arduino-songs                                            
//                                              
//                                              Robson Couto, 2019
//*/
//#define NOTE_A4  440
//#define NOTE_B4  494
//#define NOTE_CS5 554
//#define NOTE_D5  587
//#define NOTE_E5  659
//#define NOTE_FS5 740
//#define NOTE_A5  880
//#define REST      0
//
//
//// change this to make the song slower or faster
//int tempo = 114;
//
//// notes of the moledy followed by the duration.
//// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
//// !!negative numbers are used to represent dotted notes,
//// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
//int melody[] = {
//
//  // Never Gonna Give You Up - Rick Astley
//  // Score available at https://musescore.com/chlorondria_5/never-gonna-give-you-up_alto-sax
//  // Arranged by Chlorondria
//  
//  NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
//  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
//  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
//  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  //23
//  NOTE_E5,4, NOTE_D5,2, REST, 1000,
//};
//
//// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
//// there are two values per note (pitch and duration), so for each note there are four bytes
//int notes = sizeof(melody) / sizeof(melody[0]) / 2;
//
//// this calculates the duration of a whole note in ms
//int wholenote = (60000 * 4) / tempo;
//
//int divider = 0, noteDuration = 0;
//
//void playNeverGonnaGiveYouUp() {
//  // iterate over the notes of the melody.
//  // Remember, the array is twice the number of notes (notes + durations)
//  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
//
//    // calculates the duration of each note
//    divider = melody[thisNote + 1];
//    if (divider > 0) {
//      // regular note, just proceed
//      noteDuration = (wholenote) / divider;
//    } else if (divider < 0) {
//      // dotted notes are represented with negative durations!!
//      noteDuration = (wholenote) / abs(divider);
//      noteDuration *= 1.5; // increases the duration in half for dotted notes
//    }
//
//    // we only play the note for 90% of the duration, leaving 10% as a pause
//    tone(buzzer, melody[thisNote], noteDuration * 0.9);
//
//    // Wait for the specief duration before playing the next note.
//    delay(noteDuration);
//
//    // stop the waveform generation before the next note.
//    noTone(buzzer);
//  }
//}
