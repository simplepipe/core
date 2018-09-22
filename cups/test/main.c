#include <stdio.h>
#include <cups/cups.h>

int main(void)
{
        cups_dest_t *dest = NULL;
        int num_options = 0;
        cups_option_t *options = NULL;
        cups_job_t *jobs = NULL;
        int num_jobs = 0;
        int job_id = 0;
        int i;
        char buffer[1024] = {'\0'};

        cups_dest_t *dests;
        int num_dests = cupsGetDests(&dests);
        
       dest = dests;

        const char *value = cupsGetOption("printer-info", dest->num_options, dest->options);
        printf("%s (%s)\n", dest->name, value ? value : "no description");  
        printf("2\n");
        for (i = 0; i < dest->num_options; i++)
                num_options = cupsAddOption(dest->options[i].name, dest->options[i].value,
                              num_options, &options);
        printf("1\n");
        /* Create the job */
        job_id = cupsCreateJob(CUPS_HTTP_DEFAULT, dest->name, "1 Text Files",
               num_options, options);

        /* If the job is created, add 10 files */
        if (job_id > 0) {
                for (i = 1; i <= 1; i ++) {
                        printf("?\n");
                        snprintf(buffer, sizeof(buffer), "file%d.txt", i);

                        cupsStartDocument(CUPS_HTTP_DEFAULT, dest->name, job_id, buffer,
                              CUPS_FORMAT_TEXT, i == 10);

                        snprintf(buffer, sizeof(buffer),
                        "File %d\n"
                        "\n"
                        "One fish,\n"
                        "Two fish,\n"
                        "Red fish,\n"
                        "Blue fish\n", i);

                        /* cupsWriteRequestData can be called as many times as needed */
                        cupsWriteRequestData(CUPS_HTTP_DEFAULT, buffer, strlen(buffer));

                        cupsFinishDocument(CUPS_HTTP_DEFAULT, dest->name);
                }
        }

        ipp_jstate_t job_state = IPP_JOB_PENDING;

while (job_state < IPP_JOB_STOPPED)
{
        printf("here\n");
  /* Get my jobs (1) with any state (-1) */
  num_jobs = cupsGetJobs(&jobs, dest->name, 1, -1);

  /* Loop to find my job */
  job_state = IPP_JOB_COMPLETED;

  for (i = 0; i < num_jobs; i ++)
    if (jobs[i].id == job_id)
    {
      job_state = jobs[i].state;
      break;
    }

  /* Free the job array */
  cupsFreeJobs(num_jobs, jobs);

  /* Show the current state */
  switch (job_state)
  {
    case IPP_JOB_PENDING :
        printf("Job %d is pending.\n", job_id);
        break;
    case IPP_JOB_HELD :
        printf("Job %d is held.\n", job_id);
        break;
    case IPP_JOB_PROCESSING :
        printf("Job %d is processing.\n", job_id);
        break;
    case IPP_JOB_STOPPED :
        printf("Job %d is stopped.\n", job_id);
        break;
    case IPP_JOB_CANCELED :
        printf("Job %d is canceled.\n", job_id);
        break;
    case IPP_JOB_ABORTED :
        printf("Job %d is aborted.\n", job_id);
        break;
    case IPP_JOB_COMPLETED :
        printf("Job %d is completed.\n", job_id);
        break;
  }

  /* Sleep if the job is not finished */
  if (job_state < IPP_JOB_STOPPED)
    sleep(5);
}

        cupsFreeOptions(num_options, options);
        cupsFreeDests(num_dests, dests);

        return (0);
}