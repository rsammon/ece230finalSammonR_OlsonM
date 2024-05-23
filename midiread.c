/*! \file */
/******************************************************************************
 * MSP432 Final Project
 *
 * Description: Decoding a MIDI file (.mid) while storing all useful information for the project within multiple arrays or variables
 *
 * Author: Mateo Olson, Rowan Sammon
 * Last-modified: 5/23/2024
 *
*******************************************************************************/
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

//Helper method used to get all of the info from the main header at the beginning of the track
static void mainHeaderHandler(unsigned char buffer[], uint16_t *midiFormat, uint16_t *numTracks, uint16_t *quarterNoteTicks, int *i) {
	uint32_t headerLength = 0x000000;
	(*i)++;
	headerLength += (buffer[*i] << 24); //Specify length of main header (typically 6 bytes)
	(*i)++;
	headerLength += (buffer[*i] << 16);
	(*i)++;
	headerLength += (buffer[*i] << 8);
	(*i)++;
	headerLength += buffer[*i];
	(*i)++;

	*midiFormat += (buffer[*i] << 8); //Specify MIDI formatting
	(*i)++;
	*midiFormat += buffer[*i];
	(*i)++;

	*numTracks += (buffer[*i] << 8); //Specify number of tracks
	(*i)++;
	*numTracks += buffer[*i];
	(*i)++;

	*quarterNoteTicks += (buffer[*i] << 8); //Specify number of ticks per quarter note (used for specifying delta time in events)
	(*i)++;
	*quarterNoteTicks += buffer[*i];

	printf("MAINHEADERHANDLER \n");
}

//Setting microseconds per quarter note for the first time, the rest are handled in track and sent to the lateMetaEvent array along with timing data in order to be executed later in the track
static void metaEventHandlerSetBPM(unsigned char buffer[], int* microsecQuarterNote, int* i, int* n, int metaEventLength) {
	*n += metaEventLength;
	*microsecQuarterNote += (buffer[*i] << 16);
	(*i)++;
	*microsecQuarterNote += (buffer[*i] << 8);
	(*i)++;
	*microsecQuarterNote += buffer[*i];
	(*i)++;
	printf("SetBPM %x \n", *microsecQuarterNote);
}


int main() {
	uint32_t bpm = 120; //BPM calculated from tempo set in Track Header
	int numTracks = 0; //Number of tracks within MIDI file
	int quarterNoteTicks = 0; //Number of ticks per quarter note, used for calculating delays with Meta Events and note timings
	int midiFormat = 0; //Format of MIDI file, can be 0, 1, or 2
	int curTrack = 1; //Current track, used to figure out which array notes should go into
	int lateMetaEventCounter = 0; //Used to iterate through and setup late meta events
	int curNotes = 1; //Current amount of notes
	int notePointer = 0;
	//Array containing the frequencies corresponding to each MIDI note, 128 is unused in MIDI so it is used here to represent a rest as it is outside of audible range
	float frequencies[] = { 16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, 32.70, 34.65, 36.71, 41.20, 43.65, 46.25, 49.00, 51.91, 55.00, 58.37, 61.74, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.8, 110.0, 116.5, 123.5, 130.8, 138.6, 146.8, 155.6, 164.8, 174.6, 185.0, 196.0, 207.7, 220.0, 233.1, 246.9, 261.6, 277.2, 293.7, 311.1, 329.6, 349.2, 370.0, 392.0, 415.3, 440.0, 466.2, 493.9, 523.3, 554.4, 587.3, 622.3, 659.3, 698.5, 740.0, 784.0, 830.6, 880.0, 932.3, 987.8, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 1637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11839, 12543, 13290, 14080, 14917, 15804, 16744, 17740, 18795, 19912, 21096, 22351, 23680, 28160, 29834, 31609};
	uint32_t microsecQuarterNote = 0; //Microseconds per quarter note
	uint32_t microsecQuarterNoteTemp = 0; //Stored for when BPM changes at a later time
	//MIDI file read
	FILE* ptr;
	ptr = fopen(".mid", "rb");
	if (ptr == NULL) {
		fprintf(stderr, "Failed to open file\n");
		return 1;
	}

	fseek(ptr, 0, SEEK_END);
	long size = ftell(ptr);
	rewind(ptr);

	//Using calloc to declare arrays as a future plan is to make their size adjustible to fit the amount of notes
	unsigned char* buffer = (unsigned char*)calloc(size, sizeof(unsigned char));
	if (buffer == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}
	
	unsigned int* noteLCD1 = (unsigned int*)calloc(size, sizeof(unsigned int));
	if (noteLCD1 == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	unsigned int* noteLCD2 = (unsigned int*)calloc(size, sizeof(unsigned int));
	if (noteLCD2 == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}
	
	unsigned int* noteLengthsTrack1 = (unsigned int*)calloc(size, sizeof(uint32_t));
	if (noteLengthsTrack1 == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}
	
	unsigned int* noteFreqTrack1 = (unsigned int*)calloc(size, sizeof(unsigned int));
	if (noteFreqTrack1 == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	unsigned int* noteLengthsTrack2 = (unsigned int*)calloc(size, sizeof(unsigned int));
	if (noteLengthsTrack2 == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	unsigned int* noteFreqTrack2 = (unsigned int*)calloc(size, sizeof(unsigned int));
	if (noteFreqTrack2 == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}
	
	unsigned int* lateMetaEventTrigger = (unsigned int*)calloc(size, sizeof(unsigned int));
	if (lateMetaEventTrigger == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	unsigned int* lateMetaEventTiming = (unsigned int*)calloc(size, sizeof(unsigned int));
	if (lateMetaEventTiming == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	uint32_t* lateMetaEventData = (unsigned int*)calloc(size, sizeof(uint32_t));
	if (lateMetaEventData == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	unsigned int* noteTrackFreq = (unsigned int*)calloc(curNotes, sizeof(unsigned int));
	if (noteTrackFreq == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	noteTrackFreq[0] = 0x80;

	uint32_t* noteTrackLength = (unsigned int*)calloc(curNotes, sizeof(uint32_t));
	if (noteTrackLength == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	noteTrackLength[0] = 0;


	fread(buffer, sizeof(unsigned char), size, ptr); //Reading data into the buffer array
	fclose(ptr);
	uint32_t i; //Iterator for entire file, used for buffer
	uint32_t n; //Iterator for track lengths to ensure tracks are read in full
	uint32_t trackLength = 0x00000000; //To ensure tracks are fully read
	uint32_t metaEventLength = 0x00000000; //For skipping unnecessary meta events
	uint32_t deltaTimeCounter = 0; //Counter of the current delta time for note lengths and late meta events
	bool deltaTimeAcquired = false; //Switch to indicate delta timers
	bool notesStart = false; //To indicate when a noteOn event has started, signalling that only note events and meta events can occur
	int tempNote = 0;

	for (i = 0; i < size; i++) {
		printf("%02x ", buffer[i]); //Used to ensure all of the delta times are read accurately
		if (buffer[i] = 0x4D){ //Header Track
			i += 2;
			if (buffer[i] == 0x68) { //Main Header
				i++;
				mainHeaderHandler(buffer, &midiFormat, &numTracks, &quarterNoteTicks, &i);
			}
			else if (buffer[i] == 0x72 && i < size) { //Track Chunk
				i += 2;
				trackLength = 0;
				notePointer = 0;

				trackLength += (buffer[i] << 24); //Specify length of current track
				i++;
				trackLength += (buffer[i] << 16);
				i++;
				trackLength += (buffer[i] << 8);
				i++;
				trackLength += buffer[i];
				i++;
				printf("%d", curNotes);
				deltaTimeCounter = 0;
				printf("TRACKLENGTH: %d i: %d \n", trackLength, i);
				for (n = 0; n < trackLength; n++) 
				{
					printf("%02x ", buffer[i]);
					if (!deltaTimeAcquired) { //Used to read the delta time, or time between instructions
						if (buffer[i] < 0x80) {
							deltaTimeCounter += buffer[i];
							i++;
							deltaTimeAcquired = true;
						}
						else {
							deltaTimeCounter += ((buffer[i] & 0x7F) << 7); //If the delta time > 0x80, the MSB of both this byte and the next byte are removed and the two are combined
							i++;
							deltaTimeCounter += (buffer[i] & 0x7F);
							i++;
							n++;
							deltaTimeAcquired = true;
						}
					}
					
					if(buffer[i] == 0xFF){ //Meta Event
						i++;
						n++;
						
						if (deltaTimeCounter > 0) {
							switch (buffer[i]) {
							case 0x51: //Set Tempo sometime after the beginning of the track
								i++;
								n++;
								metaEventLength = buffer[i];
								i++;
								n++;
								microsecQuarterNoteTemp = 0;
								metaEventHandlerSetBPM(buffer, &microsecQuarterNoteTemp, &i, &n, metaEventLength); //Get microseconds per quarter note
								lateMetaEventTrigger[lateMetaEventCounter] = 1;
								lateMetaEventTiming[lateMetaEventCounter] = deltaTimeCounter;
								lateMetaEventData[lateMetaEventCounter] = (60000000 / microsecQuarterNoteTemp);
								lateMetaEventCounter++;
								deltaTimeAcquired = false;
								break;
							case 0x2F: //End Track
								i++;
								n = trackLength;
								deltaTimeAcquired = false;
								notesStart = false;
								break;
							default:
								i++;
								n++;
								metaEventLength = buffer[i];
								i += metaEventLength;
								n += metaEventLength;
								deltaTimeAcquired = false;
								break;
							}
						}
						else {
							switch (buffer[i]) {
							case 0x51: //Set Tempo
								i++;
								n++;
								metaEventLength = buffer[i];
								i++;
								n++;
								microsecQuarterNote = 0;
								metaEventHandlerSetBPM(buffer, &microsecQuarterNote, &i, &n, metaEventLength); //Get microseconds per quarter note
								bpm = 60000000 / microsecQuarterNote;
								printf("BPM: %d \n", bpm);
								deltaTimeAcquired = false;
								break;
							case 0x2F: //End Track
								i++;
								n++;
								deltaTimeAcquired = false;
								trackLength = 0;
								break;
							default:
								i++;
								n++;
								metaEventLength = buffer[i];
								i += metaEventLength;
								n += metaEventLength;
								i++;
								n++;
								deltaTimeAcquired = false;
								break;
							}
						}
					}
					else if (buffer[i] == 0x80) { //NoteOff
							notesStart = false;
							i++;
							n++;
							tempNote = buffer[i];
							notePointer++;
							i++;
							n++;
							curNotes--;
							noteTrackLength[0] = deltaTimeCounter;
							if (curNotes == 1) {//Going from one note to silence
								if (curTrack == 1) {
									noteFreqTrack1[(notePointer - 1)] = tempNote;
									noteLengthsTrack1[(notePointer - 1)] = deltaTimeCounter - noteTrackLength[1];
								}
								else if (curTrack == 2) {
									noteFreqTrack2[(notePointer - 1)] = tempNote;
									noteLengthsTrack2[(notePointer - 1)] = deltaTimeCounter - noteTrackLength[1];
								}
								noteTrackFreq = (unsigned int*)realloc(noteTrackFreq, curNotes * sizeof(*noteTrackFreq));
								if (noteTrackFreq == NULL) {
									fprintf(stderr, "Memory allocation failed\n");
									return 1;
								}
								noteTrackLength = (uint32_t*)realloc(noteTrackLength, curNotes * sizeof(*noteTrackLength));
								if (noteTrackLength == NULL) {
									fprintf(stderr, "Memory allocation failed\n");
									return 1;
								}
							}

							i++;
							n++;
							deltaTimeAcquired = false;
					}
					else if (buffer[i] == 0x90) { //NoteOn event
						notesStart = true;
						i++;
						n++;
						tempNote = buffer[i];
						notePointer++;
						i++;
						n++;
						if (buffer[i] != 0) { //Add a note
							curNotes++;
							noteTrackFreq = (unsigned int*)realloc(noteTrackFreq, curNotes * sizeof(*noteTrackFreq));
							if (noteTrackFreq == NULL) {
								fprintf(stderr, "Memory allocation failed\n");
								return 1;
							}
							noteTrackLength = (uint32_t*)realloc(noteTrackLength, curNotes * sizeof(*noteTrackLength));
							if (noteTrackLength == NULL) {
								fprintf(stderr, "Memory allocation failed\n");
								return 1;
							}
							noteTrackFreq[curNotes - 1] = tempNote;
							noteTrackLength[curNotes - 1] = deltaTimeCounter;
							if (curNotes == 2) {
								if (curTrack == 1) { //Track 1 going from silence to single note
									noteFreqTrack1[(notePointer - 1)] = 0x80; //Setting up the silence as a note
									noteLengthsTrack1[(notePointer - 1)] = (deltaTimeCounter - noteTrackLength[0]);
									noteLCD1[(notePointer - 1)] = ((deltaTimeCounter - noteTrackLength[0]));
								}
								else if (curTrack == 2) {
									noteFreqTrack2[(notePointer - 1)] = 0x80;
									noteLengthsTrack2[(notePointer - 1)] = (deltaTimeCounter - noteTrackLength[0]);
									noteLCD2[(notePointer - 1)] = ((deltaTimeCounter - noteTrackLength[0]));
								}
							}
						}
						else {//End a note
							curNotes--;
							noteTrackLength[0] = deltaTimeCounter;
							if (curNotes == 1) {//Going from one note to silence
								if (curTrack == 1) {
									noteFreqTrack1[(notePointer - 1)] = tempNote;
									noteLengthsTrack1[(notePointer - 1)] = deltaTimeCounter - noteTrackLength[1];
									noteLCD1[(notePointer - 1)] = ((deltaTimeCounter - noteTrackLength[1]) | 0x80000000);
								}
								else if (curTrack == 2) {
									noteFreqTrack2[(notePointer - 1)] = tempNote;
									noteLengthsTrack2[(notePointer - 1)] = deltaTimeCounter - noteTrackLength[1];
									noteLCD2[(notePointer - 1)] = ((deltaTimeCounter - noteTrackLength[1]) | 0x80000000);
								}
								noteTrackFreq = (unsigned int*)realloc(noteTrackFreq, curNotes * sizeof(*noteTrackFreq));
								if (noteTrackFreq == NULL) {
									fprintf(stderr, "Memory allocation failed\n");
									return 1;
								}
								noteTrackLength = (uint32_t*)realloc(noteTrackLength, curNotes * sizeof(*noteTrackLength));
								if (noteTrackLength == NULL) {
									fprintf(stderr, "Memory allocation failed\n");
									return 1;
								}
							}
						}
						i++;
						n++;
						deltaTimeAcquired = false;
					}
					else {
						switch (buffer[i] & 0xF0) { //NoteOn
						case 0xA0: //Doesn't matter
							i += 3;
							n += 3;
							deltaTimeAcquired = false;
							break;
						case 0xB0: //Doesn't matter
							i += 3;
							n += 3;
							deltaTimeAcquired = false;
							break;
						case 0xC0: //Doesn't matter
							i += 2;
							n += 2;
							deltaTimeAcquired = false;
							break;
						case 0xD0: //Doesn't matter
							i += 2;
							n += 2;
							deltaTimeAcquired = false;
							break;
						case 0xE0: //Doesn't matter
							i += 3;
							n += 3;
							deltaTimeAcquired = false;
							break;
						default: //Skipping misc. controller events
							if (buffer[i] > 0x80) {
								i += 2;
								n += 2;
							}
							else if (notesStart) {
								tempNote = buffer[i];
								i++;
								n++;
								notePointer++;
								if (buffer[i] != 0) { //Add a note
									curNotes++;

									noteTrackFreq = (unsigned int*)realloc(noteTrackFreq, curNotes * sizeof(*noteTrackFreq));
									if (noteTrackFreq == NULL) {
										fprintf(stderr, "Memory allocation failed\n");
										return 1;
									}
									noteTrackLength = (uint32_t*)realloc(noteTrackLength, curNotes * sizeof(*noteTrackLength));
									if (noteTrackLength == NULL) {
										fprintf(stderr, "Memory allocation failed\n");
										return 1;
									}
									noteTrackFreq[curNotes - 1] = tempNote;
									noteTrackLength[curNotes - 1] = deltaTimeCounter;
									if (curNotes == 2) {
										if (curTrack == 1) { //Track 1 going from silence to single note
											noteFreqTrack1[(notePointer - 1)] = 0x80; //Setting up the silence as a note
											noteLengthsTrack1[(notePointer - 1)] = (deltaTimeCounter - noteTrackLength[0]);
											noteLCD1[(notePointer - 1)] = ((deltaTimeCounter - noteTrackLength[0]) | 0x8000);
										}
										else if (curTrack == 2) {
											noteFreqTrack2[(notePointer - 1)] = 0x80;
											noteLengthsTrack2[(notePointer - 1)] = (deltaTimeCounter - noteTrackLength[0]);
											noteLCD2[(notePointer - 1)] = ((deltaTimeCounter - noteTrackLength[0]) | 0x8000);
										}
									}
								}
								else {//End a note
									noteTrackLength[0] = deltaTimeCounter;
									curNotes--;
									if (curNotes == 1) {//Going from one note to silence
										if (curTrack == 1) {
											noteFreqTrack1[(notePointer - 1)] = tempNote;
											noteLengthsTrack1[(notePointer - 1)] = (deltaTimeCounter - noteTrackLength[1]);
											noteLCD1[(notePointer - 1)] = ((deltaTimeCounter - noteTrackLength[1]) | 0x80000000);
										}
										else if (curTrack == 2) {
											noteFreqTrack2[(notePointer - 1)] = tempNote;
											noteLengthsTrack2[(notePointer - 1)] = (deltaTimeCounter - noteTrackLength[1]);
											noteLCD2[(notePointer - 1)] = ((deltaTimeCounter - noteTrackLength[1]) | 0x80000000);
										}
										noteTrackFreq = (unsigned int*)realloc(noteTrackFreq, curNotes * sizeof(*noteTrackFreq));
										if (noteTrackFreq == NULL) {
											fprintf(stderr, "Memory allocation failed\n");
											return 1;
										}
										noteTrackLength = (uint32_t*)realloc(noteTrackLength, curNotes * sizeof(*noteTrackLength));
										if (noteTrackLength == NULL) {
											fprintf(stderr, "Memory allocation failed\n");
											return 1;
										}
									}
								
								}
								i++;
								n++;
							}
							else {
								i += 2;
								n += 2;
							}
							deltaTimeAcquired = false;
							break;
						}
					}
				}
				
				
			}
		}
	}

	printf("FREQUENCYARRAY: {"); //Freq array
	for (int x = 0; x < notePointer - 1; x++) {
		printf("%.0f, ", 48000000/(frequencies[noteFreqTrack1[x]])); //Adjust for 48MHz
	}
	printf("%.0f", 48000000 / (frequencies[noteFreqTrack1[notePointer - 1]]));
	printf("} \n");

	printf("PERIODARRAY: {"); //Period array
	for (int x = 0; x < notePointer - 1; x++) {
		printf("%d, ", noteLengthsTrack1[x]);
	}
	printf("%d", noteLengthsTrack1[notePointer - 1]);
	printf("} \n");



}



