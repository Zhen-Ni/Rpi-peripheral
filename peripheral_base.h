#ifndef PERIPHERAL_BASE_H
#define PERIPHERAL_BASE_H

#include <cstdint>
#include <cstdio>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// Base address of peripherals for RPi
#define BCM_PERIPHERAL_BASE 0x3F000000

#define BLOCK_SIZE 1024 * 4

// Get access to the registers
#define ACCESS(offset) *(address() + (offset))
#define ACCESS64(offset) *(volatile uint64_t *)(address() + (offset))

class Peripheral {
  volatile uint32_t *address_p;

  void *mapmem(uint32_t base_address) const;
  void unmapmem(void *map) const;

  Peripheral(const Peripheral &) = delete;
  Peripheral &operator=(const Peripheral &) = delete;
  
public:
  Peripheral(uint32_t base_address);
  ~Peripheral();
  volatile uint32_t *const &address() const {return address_p;}
};

void *Peripheral::mapmem(uint32_t base_address) const {
  int memfd;
  if ((memfd = open("/dev/mem", O_RDWR|O_SYNC)) < 0) {
    perror("Failed to open /dev/mem, try using sudo");
    close(memfd);
    return MAP_FAILED;
  }
  
  void *map;
  map = mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED,
	     memfd, base_address);
  close(memfd);
  return map;
}

void Peripheral::unmapmem(void* map) const {
  munmap(map, BLOCK_SIZE);
}

Peripheral::Peripheral(uint32_t base_address): address_p(nullptr) {
  // Map the registers into memory
  void *map = mapmem(base_address);
  if (map == MAP_FAILED) {
    perror("Failed to map the registers into virtual memory space");
    return;
  }
  address_p = static_cast<volatile uint32_t *>(map);
}

Peripheral::~Peripheral() {
  unmapmem(static_cast<void *>(const_cast<uint32_t *>(address_p)));
}

#endif
