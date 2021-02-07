/* This is a project that I made for my Reverse Engineering and Hardware Security
 * class. I was tasked to make an application that analyzes and stores information
 * about binary files, specifically Elf files. Analysis includes performing an SHA-1 hash
 * of the file's text section, counting the number of unique assembly instructions in the file,
 * calculating the renyi quadratic entropy of the file, and comparing the analysis to a previous
 * analysis. The results of the analysis are encrypted and stored as a .bin file in a separate
 * directory. The project is designed with an exploitable vulnerability because the second part
 * of the project was to hack and write a report on another student's application.
 */

#include <libelf.h>
#include <gelf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <openssl/evp.h>
#include <capstone/capstone.h>
#include <math.h>

extern double SquareValue(double x);

#define MAX_BUF_SIZE 0x100000
typedef struct {
    int count;
    char mnemonic[50];
} Instruction;

void analyze() {
    int fd;
    Elf *e;
    GElf_Shdr shdr;
    GElf_Ehdr ehdr;
    Elf_Scn *scn;
    Elf_Data *textdata;
    size_t shstrndx;
    char *name;
    char path[100];
    EVP_MD_CTX *mdctx;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    char analysis[100];
    FILE *outfile;
    FILE *file;
    char outbuf[MAX_BUF_SIZE];

    // Asks for file name and initializes analysis file name.
    printf("Please enter the file path for the file to analyze: \n");
    scanf("%s", path);
    strcpy(analysis, path);
    strcat(analysis, "analysis.bin");
    memset(outbuf, 0, sizeof(outbuf));
    remove(analysis);
    outfile = fopen(analysis, "ab+");

    //Initialize libelf
    if (elf_version(EV_CURRENT) == EV_NONE) {
        errx(EXIT_FAILURE, "ELF library init failure: %s\n", elf_errmsg(-1));
    }

    // Attempts to open the file
    if ((fd = open(path, O_RDONLY, 0)) < 0)
    {
        err(EXIT_FAILURE, "open %s failed\n", path);
    }

    // Initialize the elf object
    if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL)
    {
        errx(EXIT_FAILURE, "ELF begin failed: %s\n", elf_errmsg(-1));
    }

    // REQ 1: Confirms that file type is an elf file
    if (elf_kind(e) != ELF_K_ELF) 
    {
        errx(EXIT_FAILURE,"\"%s\" is not an ELF object.",path);
    }

    // Get the elf header
    if (gelf_getehdr(e, &ehdr) == NULL)
    {
        errx(EXIT_FAILURE, "getehdr failed: %s\n", elf_errmsg(-1));
    }

    int textaddr = 0;

    // REQ 8: Uses libelf to start looking for text section of elf file.
    if (elf_getshdrstrndx(e, &shstrndx) != 0) 
    {
        errx(EXIT_FAILURE,"elf_getshdrstrndx() failed: %s.",elf_errmsg(-1));
    }
    scn = NULL;
    while((scn = elf_nextscn(e, scn)) != NULL) {
        if(gelf_getshdr(scn , &shdr) != &shdr) {
            errx(EXIT_FAILURE,"getshdr() failed: %s.", elf_errmsg(-1));
        }
        if ((name = elf_strptr(e, shstrndx , shdr.sh_name)) == NULL) {
            errx(EXIT_FAILURE , "elf_strptr() failed: %s." ,elf_errmsg(-1));
        }
        // REQ 2.5: Developer Defined Function: prints out the size of each section
        printf("Section %s size: %lu bytes\n", name, shdr.sh_size);
        fprintf(outfile,"Section %s size: %lu bytes\n", name, shdr.sh_size);

        if (!strcmp(name,".text")) { // Save information about text section
            textaddr = shdr.sh_addr;
            textdata = elf_getdata(scn, textdata);
        }
    }

    // REQ 2.1 and 9: Perform SHA-1 hash
    mdctx = EVP_MD_CTX_create(); // this is EVP_MD_CTX_new in newer versions
    if (0 == EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL))  // setup an SHA-1 hash
    {
        err(EXIT_FAILURE, "Failed to init SHA-1\n");
    }
    if (0 == EVP_DigestUpdate(mdctx, textdata->d_buf, textdata->d_size)) // do one hash update
    {
        err(EXIT_FAILURE, "SHA-1 failed\n");
    }
    EVP_DigestFinal_ex(mdctx, md_value, &md_len); // finalize the hash

    printf("SHA-1: ");
    fprintf(outfile, "SHA-1: ");

    for (int i = 0; i < md_len; i++)
    {
        printf("%02x", md_value[i]);
        fprintf(outfile,"%02x", md_value[i]);
    }
    printf("\n");
    fprintf(outfile,"\n");
    EVP_MD_CTX_destroy(mdctx); // this is EVP_MD_CTX_free in newer versions

    // REQ 2.2 and 10: Get unique instruction counts
    csh handle;
    cs_insn *insn;
    size_t count;
    Instruction instr[1053];

    for (int i = 0; i < 1053; i++) {
        instr[i].count = 0;
    }
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        err(EXIT_FAILURE,"ERROR: Failed to initialize engine!\n");
        return;
    }

    count = cs_disasm(handle, textdata->d_buf, textdata->d_size, textaddr, 0, &insn);
    
    if (count > 0)
    {
        size_t j;

        for (j = 0; j < count; j++) {
            // If mnemonic hasn't been set, set mnemonic and increase count
            if (!strcmp(instr[insn[j].id].mnemonic, "")) {
                strcpy(instr[insn[j].id].mnemonic, insn[j].mnemonic);
            }
            instr[insn[j].id].count++;
        }

        cs_free(insn, count);
    }
    else
    {
        err(EXIT_FAILURE,"ERROR: Failed to disassemble given code!\n");
    }

    cs_close(&handle);
    // Print out unique instructions count
    printf("Count of Unique Instructions: \n");
    fprintf(outfile,"Count of Unique Instructions: \n");
    int k = 0;
    while (k < 1053) {
        if (instr[k].count > 0) {
            printf("%s, %d\n", instr[k].mnemonic, instr[k].count);
            fprintf(outfile,"%s, %d\n", instr[k].mnemonic, instr[k].count);
        }
        k++;
    }
    // REQ 2.3: Perform Renyi quadratic entropy calculation
    uint32_t entropycount[256];
    int m = 0;
    char *buffer = textdata->d_buf;
    // Get byte mapping count for text section
    while (m < textdata->d_size) {
        int y = buffer[m];
        if (y < 0) {
            y = y * -1;
        }
        entropycount[y]++;
        m++;
    }
    m = 0;
    double p = 0;
    double total = 0;
    // Perform entropy calculation
    for (;m < 256;m++) {
        p = (double)entropycount[m] / (double)textdata->d_size;
        total = total + SquareValue(p); // REQ 12: Assembly function used to square values
    }
    double entropy = (-1) * (log(total)/log(256));
    printf("Renyi quadratic entropy: %f\n", entropy);
    fprintf(outfile,"Renyi quadratic entropy: %f\n", entropy);
    close(fd);
    fclose(outfile);

    // REQ 4: Perform encryption by xor all bytes with 0xAA
    file = fopen(analysis, "rb+");
    fread(outbuf, sizeof(outbuf), 1, file);
    for (int i = 0; i < strlen(outbuf); i++) {
        outbuf[i] = (char)(outbuf[i] ^ 0xAA);
    }
    fclose(file);
    remove(analysis);
    // REQ 3: Write encrypted buffer to the file
    // Each .bin file contains a separate binary analysis
    file = fopen(analysis, "ab+");
    fwrite(outbuf, strlen(outbuf), 1, file);
    fclose(file);

    // Extra Credit 3: Compare analysis to previous analysis
    char c;
    char comppath[100];
    char compbuf[MAX_BUF_SIZE];
    memset(outbuf, 0, sizeof(outbuf));
    memset(compbuf, 0, sizeof(compbuf));
    int distance = 0;
    FILE *comp;
    printf("Analysis Complete. Would you like to compare this analysis to a previous analysis (Y/N)?\n");
    scanf(" %c", &c);
    if (c == 'Y' || c == 'y') {
        printf("Please enter the .bin file you would like to compare this analysis to:\n");
        scanf("%s", comppath);
        comp = fopen(comppath, "rb+");
        if (comp != NULL) {
            file = fopen(analysis, "rb+");
            fread(outbuf, sizeof(outbuf) ,1,file);
            fread(compbuf, sizeof(compbuf) ,1,comp);
            int min = 0;
            int dif = 0;
            if (strlen(outbuf) < strlen(compbuf)) {
                min = strlen(outbuf);
                dif = strlen(compbuf) - strlen(outbuf);
            } else {
                min = strlen(compbuf);
                dif = strlen(outbuf) - strlen(compbuf);
            }
            for (int j = 0; j < min; j++) {
                if ((char)compbuf[j] != (char)outbuf[j]) {
                    distance++;
                }
            }
            distance = distance + dif;
            printf("Levenshtein distance between the two binaries is: %d\n", distance);
        } else {
            printf("File path does not exist.\n");
        }
    }
    return;
}

void retrieve() {
    // REQ 6: method to retrieve analyses
    char path[100];
    bool invalid = true;
    char buf[MAX_BUF_SIZE];
    FILE * fd;
    printf("\n");
    // Asks for file to print analysis of
    while (invalid) {
        printf("Please enter the analysis that you would like to view: \n");
        scanf("%s", path);
        fd = fopen(path, "rb");
        if (fd != NULL) {
            invalid = false;
            fread(buf, sizeof(buf) ,1,fd);
            for (int i = 0; i < strlen(buf); i++) {
                // Print contents while decrypting file
                printf("%c", (char)(buf[i] ^ 0xAA));
            }
        } else {
            printf("Could not view file. Please try again.\n");
        }
    }
    printf("\n");
}

void retrieve_check() {
    char username[50];
    char password[50];
    bool invalid = true;

    // REQ 5: Prompt for username and password
    while (invalid) {
        printf("\nPlease enter username: ");
        scanf("%s", username);
        if (!strcmp(username, "userAccount")) {
            printf("\nPlease enter password: ");
            scanf("%s", password);
            //REQ 11: INTENTIONAL VULNERABILITY: Password can be overridden by passing in string
            // with same length as actual password
            if (strlen(password) == 16 || !strcmp(password, "passwordDROWSSAP")) {
                invalid = false;
                retrieve();
            } else {
                printf("\nInvalid password, please try again.");
            }
        } else if (!strcmp(username, "adminAccount")) {
            printf("\nPlease enter password: ");
            scanf("%s", password);
            if (strlen(password) == 26 || !strcmp(password, "4dm1np455w0rddr0w554pn1md4")) {
                invalid = false;
                retrieve();
            } else {
                printf("\nInvalid password, please try again.");
            }

        } else {
            printf("\nInvalid username, please try again.");
        }
    }
}

void delete() {
    // REQ 6: method to delete files
    char path[50];
    bool invalid = true;
    printf("\n");
    // Prompts user for file to delete
    while (invalid) {
        printf("Please enter the analysis that you would like to delete: \n");
        scanf("%s", path);
        if (remove(path) == 0) {
            invalid = false;
            printf("File deleted successfully.\n");
        } else {
            printf("Could not delete file. Please try again.\n");
        }
    }
}

void delete_check() {
    char username[50];
    char password[50];
    bool invalid = true;

    //REQ 5: Prompt for username and password
    while (invalid) {
        printf("\nPlease enter username: ");
        scanf("%s", username);
         if (!strcmp(username, "adminAccount")) {
            printf("\nPlease enter password: ");
            scanf("%s", password);
            //REQ 11: INTENTIONAL VULNERABILITY: Password can be overridden by passing in string
            // with same length as actual password
            if (strlen(password) == 26 || !strcmp(password, "4dm1np455w0rddr0w554pn1md4")) {
                invalid = false;
                delete();
            } else {
                printf("\nInvalid password, please try again.");
            }
        } else {
            printf("\nInvalid username, please try again.");
        }
    }
}

int main() {
    int option;
    bool invalid = true;
    // Determine what the user wants to do
    printf("Welcome to the Binary Analysis Program!\n");
    printf("Enter 0 to analyze a binary file.\n");
    printf("Enter 1 to retrieve previous file information.\n");
    printf("Enter 2 to delete file information (requires admin privileges).\n");
    scanf("%d",&option);

    // Enter appropriate method depending on user selection.
    while (invalid) {
        if (option == 0) {
            analyze();
            invalid = false;
        } else if (option == 1) {
            retrieve_check();
            invalid = false;
        } else if (option == 2) {
            delete_check();
            invalid = false;
        } else {
            printf("Invalid option choice. Please try again.\n");
            scanf("%d",&option);
        }     
    }
    printf("Thank you for using the Binary Analysis Program!\n");
    return EXIT_SUCCESS;

}