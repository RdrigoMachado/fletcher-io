#include "utils.h"
    

// DumpFieldToFile: dumps array into a file using RFS format


void DumpFieldToFile(int sx, int sy, int sz,
		     int ixStart, int ixEnd,
		     int iyStart, int iyEnd,
		     int izStart, int izEnd,
		     float d1, float d2, float d3,
		     float *arrP, char *fName) {

//PPL  int ix, iy, iz;
  int iy, iz;
  
  // header and binary file names
  
  char fNameHeader[128];
  strcpy(fNameHeader,fName);
  strcat(fNameHeader,".rsf");
  char fNameBinary[128];
  strcpy(fNameBinary,fNameHeader);
  strcat(fNameBinary,"@");

  // create header file in rsf format
  
  FILE *fp;
  fp=fopen(fNameHeader, "w+");
  fprintf(fp,"in=\"./%s\"\n", fNameBinary);
  fprintf(fp,"data_format=\"native_float\"\n");
  fprintf(fp,"esize=%lu\n", sizeof(float)); 
  fprintf(fp,"n1=%d\n",ixEnd-ixStart+1);
  fprintf(fp,"d1=%f\n",d1);
  fprintf(fp,"n2=%d\n",iyEnd-iyStart+1);
  fprintf(fp,"d2=%f\n",d2);
  fprintf(fp,"n3=%d\n",izEnd-izStart+1);
  fprintf(fp,"d3=%f\n",d3);
  fclose(fp);

  // create binary file
  
  fp=fopen(fNameBinary, "w+b");
  for (iz=izStart; iz<=izEnd; iz++)
    for (iy=iyStart; iy<=iyEnd; iy++) 
      fwrite((void *) (arrP +ind(ixStart,iy,iz)), sizeof(float), ixEnd-ixStart+1, fp);
  fclose(fp);
  int fsize=(ixEnd-ixStart+1)*(iyEnd-iyStart+1)*(izEnd-izStart+1);
  printf("created header (%s) and binary (%s) files\n",
	 fNameHeader,fNameBinary);
  printf("file has %d floats and %lu Mbytes from (%d,%d,%d) to (%d,%d,%d)\n",
	 fsize,fsize*sizeof(float)/(1024*1024),ixStart,iyStart,izStart,ixEnd,iyEnd,izEnd);
}


// DumpSlicePtr: prints the components of an SlicePtr variable


void DumpSlicePtr(SlicePtr p){
  printf("File %s contains time evolution of (%d:%d,%d:%d,%d:%d)\n",
	 p->fNameHeader,
	 p->ixStart, p->ixEnd, 
	 p->iyStart, p->iyEnd, 
	 p->izStart, p->izEnd);
}


// OpenSliceFile: open file in RFS format that will be continuously appended


SlicePtr OpenSliceFile(int ixStart, int ixEnd,
		       int iyStart, int iyEnd,
		       int izStart, int izEnd,
		       float dx, float dy, float dz, float dt,
		       char *fName) {
//PPL  char procName[128]="**(OpenSliceFile)**";
  SlicePtr ret;
  ret = (SlicePtr) malloc(sizeof(Slice));

  // verify slice direction

  if (ixStart==ixEnd)
    ret->direction=XSLICE;
  else if (iyStart==iyEnd)
    ret->direction=YSLICE;
  else if (izStart==izEnd)
    ret->direction=ZSLICE;
  else {
    ret->direction=FULL;
  }

  // header and binary file names
  
  strcpy(ret->fName,fName);
  strcpy(ret->fNameHeader,fName);
  strcat(ret->fNameHeader,".rsf");
  // strcpy(ret->fNameBinary,FNAMEBINARYPATH);
  // strcat(ret->fNameBinary,ret->fNameHeader);
  // strcat(ret->fNameBinary,"@");

  strcpy(ret->fName, fName);

  // create header and binary files in rsf format
  
  ret->fpHead=fopen(ret->fNameHeader, "w+");
  // ret->fpBinary=fopen(ret->fNameBinary, "w+");
  ret->ixStart=ixStart;
  ret->ixEnd=ixEnd;
  ret->iyStart=iyStart;
  ret->iyEnd=iyEnd;
  ret->izStart=izStart;
  ret->izEnd=izEnd;
  ret->itCnt=0;
  ret->dx=dx;
  ret->dy=dy;
  ret->dz=dz;
  ret->dt=dt;

  char sName[16];
  switch(ret->direction) {
  case XSLICE:
    strcpy(sName,"XSlice");
    break;
  case YSLICE:
    strcpy(sName,"YSlice");
    break;
  case ZSLICE:
    strcpy(sName,"ZSlice");
    break;
  case FULL:
    strcpy(sName,"Grid Section");
    break;
  }
  return(ret);
}


// DumpSliceFile: appends one array to an opened RFS file 


void DumpSliceFile(int sx, int sy, int sz,
		   float *arrP, SlicePtr p) {

// //PPL  int ix, iy, iz;
//   int iy, iz;
  
//   // dump section to binary file
  
//   for (iz=p->izStart; iz<=p->izEnd; iz++)
//     for (iy=p->iyStart; iy<=p->iyEnd; iy++) 
//       fwrite((void *) (arrP+ind(p->ixStart,iy,iz)),
// 	     sizeof(float),
// 	     p->ixEnd-p->ixStart+1,
// 	     p->fpBinary);

    MPI_enviar_onda(sx, sy, sz, arrP);

  // increase it count
  
  p->itCnt++;
}


void DumpSliceFile_Parallel(int sx, int sy, int sz,
		   float *arrP, SlicePtr p, int it) {

//PPL  int ix, iy, iz;
  int iy, iz;
  
  // dump section to binary file

  char partFileName[256], partFileNameBin[256], partFileNameHeader[256];
  sprintf(partFileName, "%s%s%d", p->fName, "_part_", it);
  // sprintf(partFileNameBin, "%s%d.rsf@", "TTI_part_", it);
  sprintf(partFileNameBin, "%s%s", partFileName, ".rsf@");
  sprintf(partFileNameHeader, "%s%s", partFileName, ".rsf");

  // printf("filename: %s\n", partFileName);

  FILE* outFile = fopen(partFileNameBin, "wb");
  
  if (outFile == NULL) {
      printf("Error opening output file: %s\n", partFileNameBin);
      // fclose(inFile);
      return;
  }
  
  for (iz=p->izStart; iz<=p->izEnd; iz++) {
    for (iy=p->iyStart; iy<=p->iyEnd; iy++) {
      fwrite((void *) (arrP+ind(p->ixStart,iy,iz)),
	     sizeof(float),
	     p->ixEnd-p->ixStart+1,
       outFile);
    }
  }

  fclose(outFile);

  CloseSlicePartFile(p, partFileNameBin, partFileNameHeader);

  // increase it count
  
  // p->itCnt++;
}

void DumpSliceFile_Parallel_Nofor(int sx, int sy, int sz,
		   float *arrP, SlicePtr p, int it) {

//PPL  int ix, iy, iz;
  int iy, iz;
  int totalSize = sx * sy * sz;

  char diskpath[256];

  if (getenv("DISKP1") != NULL) {
    strcpy(diskpath, getenv("DISKP1"));
  } else {
    strcpy(diskpath, "./");
  }

  // printf("Disk path: %s\n", diskpath);
  
  // dump section to binary file

  char partFileName[256], partFileNameBin[256], partFileNameHeader[256];
  sprintf(partFileName, "%s%s%s%d", diskpath, p->fName, "_part_", it);
  // sprintf(partFileNameBin, "%s%d.rsf@", "TTI_part_", it);
  sprintf(partFileNameBin, "%s%s", partFileName, ".rsf@");
  sprintf(partFileNameHeader, "%s%s", partFileName, ".rsf");

  // printf("filename: %s\n", partFileName);

  FILE* outFile = fopen(partFileNameBin, "wb");
  
  if (outFile == NULL) {
      printf("Error opening output file: %s\n", partFileNameBin);
      // fclose(inFile);
      return;
  }
  
  fwrite((void *) arrP,
	     sizeof(float),
	     totalSize,
	     outFile);

  fclose(outFile);

  CloseSlicePartFile(p, partFileNameBin, partFileNameHeader);

  // increase it count
  
  // p->itCnt++;
}


// CloseSliceFile: close file in RFS format that has been continuously appended


void CloseSliceFile(SlicePtr p){

  fprintf(p->fpHead,"in=\"%s\"\n", p->fNameBinary);
  fprintf(p->fpHead,"data_format=\"native_float\"\n");
  fprintf(p->fpHead,"esize=%lu\n", sizeof(float)); 
  switch(p->direction) {
  case XSLICE:
    fprintf(p->fpHead,"n1=%d\n",p->iyEnd-p->iyStart+1);
    fprintf(p->fpHead,"n2=%d\n",p->izEnd-p->izStart+1);
    // fprintf(p->fpHead,"n3=%d\n",p->itCnt);
    fprintf(p->fpHead,"d1=%f\n",p->dy);
    fprintf(p->fpHead,"d2=%f\n",p->dz);
    fprintf(p->fpHead,"d3=%f\n",p->dt);
    break;
  case YSLICE:
    fprintf(p->fpHead,"n1=%d\n",p->ixEnd-p->ixStart+1);
    fprintf(p->fpHead,"n2=%d\n",p->izEnd-p->izStart+1);
    // fprintf(p->fpHead,"n3=%d\n",p->itCnt);
    fprintf(p->fpHead,"d1=%f\n",p->dx);
    fprintf(p->fpHead,"d2=%f\n",p->dz);
    fprintf(p->fpHead,"d3=%f\n",p->dt);
    break;
  case ZSLICE:
    fprintf(p->fpHead,"n1=%d\n",p->ixEnd-p->ixStart+1);
    fprintf(p->fpHead,"n2=%d\n",p->iyEnd-p->iyStart+1);
    // fprintf(p->fpHead,"n3=%d\n",p->itCnt);
    fprintf(p->fpHead,"d1=%f\n",p->dx);
    fprintf(p->fpHead,"d2=%f\n",p->dy);
    fprintf(p->fpHead,"d3=%f\n",p->dt);
    break;
  case FULL:
    fprintf(p->fpHead,"n1=%d\n",p->ixEnd-p->ixStart+1);
    fprintf(p->fpHead,"n2=%d\n",p->iyEnd-p->iyStart+1);
    fprintf(p->fpHead,"n3=%d\n",p->izEnd-p->izStart+1);
    // fprintf(p->fpHead,"n4=%d\n",p->itCnt);
    fprintf(p->fpHead,"d1=%f\n",p->dx);
    fprintf(p->fpHead,"d2=%f\n",p->dy);
    fprintf(p->fpHead,"d3=%f\n",p->dz);
    fprintf(p->fpHead,"d4=%f\n",p->dt);
    break;
  }
  fclose(p->fpHead);
  // fclose(p->fpBinary);
}


// CloseSliceFile: close file in RFS format that has been continuously appended


void CloseSlicePartFile(SlicePtr p, char *partFileNameBin, char *partFileNameHeader){

  FILE* partFileHeader = fopen(partFileNameHeader, "w");

  // printf("part filename: %s\n", partFileNameHeader);
  
  if (partFileHeader == NULL) {
      printf("Error opening output file: %s\n", partFileNameHeader);
      // fclose(inFile);
      return;
  }

  fprintf(partFileHeader,"in=\"%s\"\n", partFileNameBin);
  fprintf(partFileHeader,"data_format=\"native_float\"\n");
  fprintf(partFileHeader,"esize=%lu\n", sizeof(float)); 
  switch(p->direction) {
  case XSLICE:
    fprintf(partFileHeader,"n1=%d\n",p->iyEnd-p->iyStart+1);
    fprintf(partFileHeader,"n2=%d\n",p->izEnd-p->izStart+1);
    //fprintf(partFileHeader,"n3=%d\n",p->itCnt);
    fprintf(partFileHeader,"d1=%f\n",p->dy);
    fprintf(partFileHeader,"d2=%f\n",p->dz);
    fprintf(partFileHeader,"d3=%f\n",p->dt);
    break;
  case YSLICE:
    fprintf(partFileHeader,"n1=%d\n",p->ixEnd-p->ixStart+1);
    fprintf(partFileHeader,"n2=%d\n",p->izEnd-p->izStart+1);
    //fprintf(partFileHeader,"n3=%d\n",p->itCnt);
    fprintf(partFileHeader,"d1=%f\n",p->dx);
    fprintf(partFileHeader,"d2=%f\n",p->dz);
    fprintf(partFileHeader,"d3=%f\n",p->dt);
    break;
  case ZSLICE:
    fprintf(partFileHeader,"n1=%d\n",p->ixEnd-p->ixStart+1);
    fprintf(partFileHeader,"n2=%d\n",p->iyEnd-p->iyStart+1);
    //fprintf(partFileHeader,"n3=%d\n",p->itCnt);
    fprintf(partFileHeader,"d1=%f\n",p->dx);
    fprintf(partFileHeader,"d2=%f\n",p->dy);
    fprintf(partFileHeader,"d3=%f\n",p->dt);
    break;
  case FULL:
    fprintf(partFileHeader,"n1=%d\n",p->ixEnd-p->ixStart+1);
    fprintf(partFileHeader,"n2=%d\n",p->iyEnd-p->iyStart+1);
    fprintf(partFileHeader,"n3=%d\n",p->izEnd-p->izStart+1);
    // fprintf(partFileHeader,"n4=%d\n",p->itCnt);
    fprintf(partFileHeader,"d1=%f\n",p->dx);
    fprintf(partFileHeader,"d2=%f\n",p->dy);
    fprintf(partFileHeader,"d3=%f\n",p->dz);
    fprintf(partFileHeader,"d4=%f\n",p->dt);
    break;
  }
  // fclose(p->fpHead);
  fclose(partFileHeader);
}


// DumpSliceSummary: prints info of one array 


void   DumpSliceSummary(int sx, int sy, int sz,
			SlicePtr p,
			float dt, int it, float *arrP, float src) {

  int ix, iy, iz;
  float maxP, minP, valP;
  maxP=minP=arrP[ind(p->ixStart,p->iyStart,p->izStart)];
  for (iz=p->izStart; iz<=p->izEnd; iz++)
    for (iy=p->iyStart; iy<=p->iyEnd; iy++) 
      for (ix=p->ixStart; ix<=p->ixEnd; ix++) {
	valP=arrP[ind(ix,iy,iz)];
	maxP=fmaxf(maxP,valP);
	minP=fminf(minP,valP);
      }
  printf("Slice of %s at iteration; %5.5d; time(s); %f; max; %9.2e; min; %9.2e; src; %9.2e;\n",
	 p->fNameHeader, it, dt*(float)it, maxP, minP, src);
}


// SwapArrays: swap array pointers on time forward array propagation


void SwapArrays(float * restrict *pp, float * restrict *pc, float * restrict *qp, float * restrict *qc) {
  float *tmp;

  tmp=*pp;
  *pp=*pc;
  *pc=tmp;

  tmp=*qp;
  *qp=*qc;
  *qc=tmp;
}
